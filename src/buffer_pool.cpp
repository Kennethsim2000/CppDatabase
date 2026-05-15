#include "buffer_pool.h"
#include <utility>
#include <vector>

using namespace db;

BufferPoolManager::BufferPoolManager(DiskManager &disk, size_t pool_size) : disk_(disk), cache_(pool_size), nextpage_(0)
{
    pages.resize(pool_size);
    for (int i = 0; i < pool_size; i++)
    {
        free_frames_.push(i);
    }
}

PageId BufferPoolManager::fetch_next_page()
{
    return nextpage_.fetch_add(1);
}

/*
1. If page already cached in page_table, then we pin page, remove from LRUReplacer, and return
2. Else, we need a free frame. We can first try free_frames, else we can try to evict from the LRUReplacer

*/
Page *BufferPoolManager::fetch_page(PageId page_id)
{
    std::scoped_lock lock(latch_);

    auto it = page_table_.find(page_id);
    if (it != page_table_.end()) // already in page table, loaded previously
    {
        size_t frame = it->second;
        cache_.remove(frame);
        Page &p = pages[frame];
        p.pin();
        return &p; // return a pointer to the page table, so that we can possibly modify this page
    }

    // Else we need a free frame. We can first try free_frames, else we can try to evict from LRUReplacer
    if (free_frames_.empty()) // have to get from LRUReplacer
    {
        size_t evictedFrame;
        if (cache_.evict(evictedFrame))
        {
            Page &p = pages[evictedFrame];
            if (p.is_dirty())
            {
                disk_.write_page(p);
            }
            page_table_.erase(p.getId());
            p.reset();
            p.setPageId(page_id);
            disk_.read_page(page_id, p);
            page_table_[page_id] = evictedFrame;
            p.pin();
            return &p;
        }
        else
        { // no possible frames to be evicted
            return nullptr;
        }
    }
    else
    {
        size_t frameId = free_frames_.front();
        free_frames_.pop();
        Page &p = pages[frameId];
        p.reset();
        p.setPageId(page_id);
        disk_.read_page(page_id, p);
        p.pin();
        page_table_[page_id] = frameId;
        return &p;
    }
}

// Unpin page (allow eviction)
// find page in page table, decrement pin count,
void BufferPoolManager::unpin_page(PageId page_id)
{
    /* Usually the client would call fetch_page, work on it, if edit, call page->markDirty(), then they would call
unpin_page, which would decrement pin_count, and call flush_page if pin_Count is 0.*/
    std::scoped_lock lock(latch_);

    auto it = page_table_.find(page_id);
    if (it == page_table_.end())
        return;
    size_t frame = it->second;
    Page &page = pages[frame];
    page.unpin();
    if (page.pin_count() == 0)
    {
        cache_.add(frame);
    }
}

void BufferPoolManager::flush_page(PageId page_id)
{
    std::scoped_lock lock(latch_);

    auto it = page_table_.find(page_id);
    if (it == page_table_.end())
    {
        return;
    }
    size_t frame_id = it->second;
    Page &p = pages[frame_id];
    if (p.is_dirty())
    {
        disk_.write_page(p);
        p.clear_dirty();
    }
}
/*
We can first obtain the frame_id from the page_table using this pageId, obtain the page, if it is dirty, we
will call disk_manager.write_page(vector[frameId]), then we will clear out this page.data(maybe add a method
page to reset the .data(), and add this frameId to the LRUReplacer.*/

// void BufferPoolManager::flush_all()
// {
// }
