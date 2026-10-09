#pragma once

#include <vector>

#include "expression.h"
#include "table_heap.h"
#include "tuple.h"
#include "schema.h"

namespace db
{
    class SeqScanExecutor
    {
    public:
        SeqScanExecutor(
            TableHeap &table,
            const Schema &schema,
            const Expression &predicate);

        std::vector<Tuple> execute();

    private:
        TableHeap &table_;
        const Schema &schema_;
        const Expression &predicate_;
    };
}