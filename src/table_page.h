/*
One page contains many tuples
Initially:
    append-only
    no deletes
    fixed-size tuples
*/

#pragma once

#include "page.h"
#include "tuple.h"
#include "rid.h"

namespace db
{
    class TablePage
    {
    public:
        explicit TablePage(Page *page);

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
    };
}