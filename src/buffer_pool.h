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
#include <unordered_set>
#include <queue>

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
        void unpin_page(PageId page_id);

        void flush_page(PageId page_id);
        // void flush_all();

    private:
        DiskManager &disk_;
        std::mutex latch_;
        std::vector<Page> pages;                        // Fixed size pool, pages lives here permanently
        std::unordered_map<PageId, size_t> page_table_; // maps the pageId to the index in the vector
        LRUReplacer cache_;
        std::atomic<PageId> nextpage_;
        std::queue<size_t> free_frames_;
    };

} // namespace db

/*
Free frame --> Contains NO valid cached page
LRU frame --> Contains a valid page that MAY be evicted

Suppose we are keeping both in the same set
free_frames = {0,1,2}
You cannot distinguish between frame 0 is empty VS frame 0 contains page 99
So when reusing, should you flush? Should you erase page table? Is it safe?

*/