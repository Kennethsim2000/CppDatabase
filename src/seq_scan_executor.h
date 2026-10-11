#pragma once

#include <vector>

#include "executor.h"
#include "table_heap.h"
#include "schema.h"
#include "expression.h"

namespace db
{
    class SeqScanExecutor : public Executor
    {
    public:
        SeqScanExecutor(
            TableHeap &table,
            const Schema &schema,
            const Expression &predicate);

        void init() override;

        bool next(Tuple &tuple, RID &rid) override;

    private:
        TableHeap &table_;
        const Schema &schema_;
        const Expression &predicate_;

        std::vector<RID> rids_;
        size_t next_rid_index_ = 0;
    };
}