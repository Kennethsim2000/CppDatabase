#pragma once

#include <vector>

#include "buffer_pool.h"
#include "table_page.h"
#include "tuple.h"
#include "rid.h"

namespace db
{
    class TableHeap
    {
    public:
        explicit TableHeap(BufferPoolManager &bpm);

        /*
            This becomes the real database insertion
            1. Find page with space
            2. fetch page from bpm
            3. Insert tuple into TablePage
            4. mark dirty
            5. unpin
        */
        RID insert_tuple(const Tuple &tuple);

        /*
            1. Fetch page
            2. read tuple
            3. unpin
        */
        bool get_tuple(const RID &rid, Tuple &tuple);

    private:
        BufferPoolManager &bpm_;
        std::vector<PageId> pages;
    };
}