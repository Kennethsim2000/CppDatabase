/*
One page contains many tuples
Initially:
    append-only
    no deletes
    fixed-size tuples

The tablepage manages tuples inside one page

Header
slot directory
Tuple Data

TablePage only manages one physical page, not the entire table
*/

#pragma once

#include "page.h"
#include "tuple.h"
#include "rid.h"

namespace db
{
    struct TablePageHeader
    {
        uint16_t tuple_count;

        // pointer keeping track of where should the next tuple be written
        // Initial value, free space starts at the very end of the page
        uint16_t free_space_ptr;
    };

    // Suppose tuples are variable size, how do we know where tuple B starts
    // Tuple A stored at offset 4046 size 50
    // Tuple B stored at offset 3926 size 120
    struct Slot
    {
        uint16_t offset;
        uint16_t size;
    };

    class TablePage
    {
    public:
        explicit TablePage(Page *page);

        void init();

        /* 1. Compute write offset
           2. Copy bytes into page
           3. Update tuple count
           4. return RID
           5. mark page dirty*/
        bool insert_tuple(const Tuple &tuple, RID &rid);

        /*
            1. Locate tuple bytes
            2. Reconstruct Tuple object
        */
        bool get_tuple(const RID &rid, Tuple &tuple);

        // checks enough remaining bytes
        bool has_space(size_t tuple_size);

    private:
        Page *page_;

        TablePageHeader *header();

        Slot *slots();
    };
}

/*
+---------------------------+
| Header                    |
| tuple_count               |
| free_space_ptr            |
+---------------------------+
| Slot 0                    |
| Slot 1                    |
| Slot 2                    |
+---------------------------+

        FREE SPACE

+---------------------------+
| Tuple bytes               |
| Tuple bytes               |
| Tuple bytes               |
+---------------------------+
*/