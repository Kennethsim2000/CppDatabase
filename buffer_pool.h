// ================================
// File: include/db/buffer_pool.h
// Caches pages in memory and coordinates disk I/O
// ================================
#pragma once

#include <unordered_map>
#include <mutex>
#include "disk_manager.h"

namespace db
{

    class BufferPoolManager
    {
    public:
        explicit BufferPoolManager(DiskManager &disk);

        // Fetch a page into memory (pin it)
        Page *fetch_page(PageId page_id);

        // Unpin page (allow eviction)
        void unpin_page(PageId page_id, bool is_dirty);

    private:
        DiskManager &disk_;
        std::mutex latch_;

        // TODO: page table, replacement policy (LRU, Clock)
    };

} // namespace db