#include "buffer_pool.h"
#include <utility>
#include <vector>

using namespace db;

BufferPoolManager::BufferPoolManager(DiskManager &disk, size_t pool_size) : disk_(disk), capacity_(pool_size), nextpage_(0)
{
    for (int i = 0; i < pool_size; i++)
    {
        available_frames_.insert(i);
    }
}

PageId BufferPoolManager::fetch_next_page()
{
    return nextpage_.fetch_add(1);
}

Page *BufferPoolManager::fetch_page(PageId page_id)
{
    auto it = page_table_.find(page_id);
    if (it != page_table_.end()) // already in page table, loaded previously
    {
        size_t frame = page_table_[page_id];
        available_frames_.erase(frame); // this frame is no longer free for loading frames
        pages[frame].pin();
        return &pages[frame]; // return a pointer to the page table, so that we can possibly modify this page
    }

    // Need to load new page into our page table and page vector
    if (available_frames_.empty())
    { // no free frames to load in our page table
        return nullptr;
    }

    size_t frame = *available_frames_.begin();
    available_frames_.erase(frame);
    Page &p = pages[frame];

    if (p.getId() > 0) // we are evicting a valid page, flush page if dirty
    {
        if (p.is_dirty())
        {
            disk_.write_page(p);
        }
        page_table_.erase(p.getId());
    }

    disk_.read_page(page_id, p);
    p.setPageId(page_id);
    page_table_[page_id] = frame;
    p.pin();
    return &p;
}

// Unpin page (allow eviction)
void BufferPoolManager::unpin_page(PageId page_id)
{
    /* Usually the client would call fetch_page, work on it, if edit, call page->markDirty(), then they would call
unpin_page, which would decrement pin_count, and call flush_page if pin_Count is 0.*/
    auto it = page_table_.find(page_id);
    if (it == page_table_.end())
        return;
    size_t frame = it->second;
    Page &page = pages[frame];
    page.unpin();
    if (page.pin_count() == 0)
    {
        available_frames_.insert(frame);
    }
}

void BufferPoolManager::flush_page(PageId page_id)
{
    size_t frame_id = page_table_[page_id];
    if (pages[frame_id].is_dirty())
    {
        disk_.write_page(pages[frame_id]);
    }
}
/*
We can first obtain the frame_id from the page_table using this pageId, obtain the page, if it is dirty, we
will call disk_manager.write_page(vector[frameId]), then we will clear out this page.data(maybe add a method
page to reset the .data(), and add this frameId to the LRUReplacer.*/

// void BufferPoolManager::flush_all()
// {
// }
