// ================================
// File: include/db/btree.h
// B+Tree index structure
// ================================
#pragma once

#include "buffer_pool.h"

namespace db
{

    class BTree
    {
    public:
        explicit BTree(BufferPoolManager &bpm);

        // Insert a key-value pair
        void insert(int key, const Record &value);

        // Lookup a key
        bool find(int key, Record &out);

    private:
        BufferPoolManager &bpm_;

        // TODO: node split, merge, traversal logic
    };

} // namespace db