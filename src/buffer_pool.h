// ================================
// File: include/db/buffer_pool.h
// Caches pages in memory and coordinates disk I/O
// ================================
#pragma once

#include <unordered_map>
#include <mutex>
#include "disk_manager.h"
#include <vector>
#include "lru_replacer.h"
#include <atomic>

namespace db
{

    class BufferPoolManager
    {
    public:
        explicit BufferPoolManager(DiskManager &disk, size_t pool_size);

        PageId fetch_next_page();

        // Fetch a page into memory (pin it), can be used to read or write from it
        // another external process that reads/write must be in charge of marking dirty
        Page *fetch_page(PageId page_id);

        // Unpin page (allow eviction)
        void unpin_page(PageId page_id, bool is_dirty);

        void flush_page(PageId page_id);
        void flush_all();

    private:
        DiskManager &disk_;
        std::mutex latch_;
        std::vector<Page> pages;                        // Fixed size pool, pages lives here permanently
        std::unordered_map<PageId, size_t> page_table_; // maps the pageId to the index in the vector
        LRUReplacer cache_;
        std::atomic<PageId> nextpage_;
    };

} // namespace db

/*
flow
Is page_id already in the page table --> yes(fetch Page)
No --> Is there a free slot? (call DiskManager to fetch page, and store the page in the vector)
No free slot-->Can we evict something

fetch_page(new_page_id):
Pool is full, no free frames
↓
LRU says: evict frame 2
↓
pool_[2] is dirty?
→ Yes: disk_.write_page(pool_[2]) ← write happens here
↓
Reuse frame 2 for new_page_id

new_page():
1. Assign the next available PageId (0, 1, 2, 3...)
2. Find a free frame in the pool
(or evict if pool is full)
3. Zero out the buffer(prev page data)
4. Pin it
5. Add to page table
6. Return pointer to the page
*/