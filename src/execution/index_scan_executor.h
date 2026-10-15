
#pragma once

#include "index/b_plus_tree.h"
#include "table_heap.h"
#include "tuple.h"
#include "rid.h"

namespace db
{

    class IndexScanExecutor
    {
    public:
        IndexScanExecutor(
            TableHeap &table_heap,
            const BPlusTree &index,
            int key);

        void init();

        bool next(Tuple &tuple, RID &rid);

    private:
        TableHeap &table_heap_;
        const BPlusTree &index_;
        int key_;

        bool executed_ = false;
    };

}
