#include "buffer_pool.h"
#include <utility>
#include <vector>

using namespace db;

BufferPoolManager::BufferPoolManager(DiskManager &disk, size_t pool_size) : disk_(disk), pages(std::vector<Page>(pool_size)), nextpage_(0)
{
    for (int i = 0; i < pool_size; i++)
    {
        cache_.add(i);
    }
}

/* BufferPoolManager(disk, pol_size)
let us std::move disk inside our BufferPoolManager since we do not need a DiskManager outside of our
BufferPoolManager, then we will call .reserve(pool_size) on our pages, maybe call add(frame_id) on the
LRUReplacer.


fetch_page:
First check if pageId is loaded in page_table, if it is, obtain the index, and return pages[index], we need to call LRUReplacer.remove(frameId) to let it know that this page cannot be evicted.
Now if it is not within the page_table, we need to check if there is a free slot by calling LRUReplacer.evict(),
if there is a free slot, we can get the index, call disk_manager.readPage(pageId, pages[index]), as well as
populate the page_table(pageId, frame_id). Else if no free slot, return nullptr(would it be good to return
OPtional?)

unpin_page:
Usually the client would call fetch_page, work on it, if edit, call page->markDirty(), then they would call
unpin_page, which would decrement pin_count, and call flush_page if pin_Count is 0.

Flush_page:
We can first obtain the frame_id from the page_table using this pageId, obtain the page, if it is dirty, we
will call disk_manager.write_page(vector[frameId]), then we will clear out this page.data(maybe add a method
page to reset the .data(), and add this frameId to the LRUReplacer.
*/