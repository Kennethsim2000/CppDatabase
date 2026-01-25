// ================================
// File: include/db/lock_manager.h
// Concurrency control (locks)
// ================================
#pragma once

#include <mutex>
#include <unordered_map>
#include "types.h"

namespace db
{

    enum class LockMode
    {
        SHARED,
        EXCLUSIVE
    };

    class LockManager
    {
    public:
        void lock(TxnId txn, PageId page, LockMode mode);
        void unlock(TxnId txn, PageId page);

    private:
        std::mutex latch_;
        // TODO: lock table, wait-for graph (deadlock detection)
    };

} // namespace db
