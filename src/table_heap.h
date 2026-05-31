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

/*
TableHeap manages many TablePages, it represents an actual database table

Suppose users table
Page 0 -> rows 1 - 50
Page 1 -> rows 51-100
Page 2 -> rows 101-150

Example insertion flow
Tuple tuple;
table_heap.insert_tuple(tuple)

Step 1: Tableheap finds page
PageId pid = ...

Step 2: Fetch page from BPM
Page* page = bpm.fetch_page(pid)
Now, page is in memory

Step 3 -- Wrap page as TablePage
TablePage tp(page)
you are converting raw bytes into slotted-page structure

Step 4 -- Insert tuple
tp.insert_tuple(tuple, rid)

Step 5 -- Unpin page
bpm.unpin_page(pid)

TableHeap
│
├── Page 0 (TablePage)
│     ├── Tuple 0
│     ├── Tuple 1
│     └── Tuple 2
│
├── Page 1 (TablePage)
│     ├── Tuple 0
│     ├── Tuple 1
│     └── Tuple 2
│
└── Page 2 (TablePage)
      ├── Tuple 0
      └── Tuple 1
*/