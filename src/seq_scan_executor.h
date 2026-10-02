#pragma once

#include "table_heap.h"
#include "schema.h"
#include "predicate.h"

namespace db
{
    class SeqScanExecutor
    {
    public:
        SeqScanExecutor(
            TableHeap &table,
            const Schema &schema,
            const Predicate &predicate);

        std::vector<Tuple> execute();

    private:
        TableHeap &table_;
        const Schema &schema_;
        const Predicate &predicate_;
    };
}