#pragma once

#include <vector>

#include "executor.h"
#include "index/b_plus_tree.h"
#include "table_heap.h"
#include "tuple.h"
#include "rid.h"

namespace db
{

    class IndexScanExecutor : public Executor
    {
    public:
        IndexScanExecutor(
            TableHeap &table_heap,
            const BPlusTree &index,
            int key);

        void init() override;

        bool next(Tuple &tuple, RID &rid) override;

    private:
        TableHeap &table_heap_;
        const BPlusTree &index_;
        int key_;

        std::vector<RID> rids_;
        std::size_t next_rid_index_ = 0;
    };

}
