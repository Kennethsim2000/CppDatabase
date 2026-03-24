#include "buffer_pool.h"
#include <utility>
#include <vector>

using namespace db;

BufferPoolManager::BufferPoolManager(DiskManager &disk, size_t pool_size) : disk_(disk), capacity_(pool_size), nextpage_(0)
{
    for (int i = 0; i < pool_size; i++)
    {
        cache_.add(i);
    }
}

PageId BufferPoolManager::fetch_next_page()
{
    return nextpage_.fetch_add(1);
}

Page *BufferPoolManager::fetch_page(PageId page_id)
{
    /*
    First check if pageId is loaded in page_table, if it is, obtain the index, and return pages[index], we need to call LRUReplacer.remove(frameId) to let it know that this page cannot be evicted.
Now if it is not within the page_table, we need to check if there is a free slot by calling LRUReplacer.evict(),
if there is a free slot, we can get the index, call disk_manager.readPage(pageId, pages[index]), as well as
populate the page_table(pageId, frame_id). Else if no free slot, return nullptr(would it be good to return
OPtional?)*/
    if (page_table_.find(page_id) != page_table_.end())
    { // page is present in page table
        size_t index = page_table_[page_id];
        cache_.remove(index); // this frame cannot be evicted as it is being referenced
        return &pages[index]; // return a pointer to the page table, so that we can possibly modify this page
    }
    else
    {
        auto pageEvictIndexOpt = cache_.evict();
        if (pageEvictIndexOpt.has_value())
        {
            size_t pageEvictIndex = pageEvictIndexOpt.value();
            disk_.read_page(pageEvictIndex, pages[pageEvictIndex]);
            page_table_[page_id] = pageEvictIndex;
        }
        else
        { // no free frame to evict
            return nullptr;
        }
    }
}

// Unpin page (allow eviction)
void BufferPoolManager::unpin_page(PageId page_id, bool is_dirty)
{
    /* Usually the client would call fetch_page, work on it, if edit, call page->markDirty(), then they would call
unpin_page, which would decrement pin_count, and call flush_page if pin_Count is 0.*/
    size_t index = page_table_[page_id];
    pages[index].unpin();
    if (pages[index].pin_count() == 0)
    {
        flush_page(page_id);
    }
}

void BufferPoolManager::flush_page(PageId page_id)
{
    size_t frame_id = page_table_[page_id];
    if (pages[frame_id].is_dirty())
    {
        disk_.write_page(pages[frame_id]);
    }
    pages[frame_id].reset();
    cache_.add(frame_id);
}
/*
We can first obtain the frame_id from the page_table using this pageId, obtain the page, if it is dirty, we
will call disk_manager.write_page(vector[frameId]), then we will clear out this page.data(maybe add a method
page to reset the .data(), and add this frameId to the LRUReplacer.*/

// void BufferPoolManager::flush_all()
// {
// }
