#include "seq_scan_executor.h"

using namespace db;

SeqScanExecutor::SeqScanExecutor(
    TableHeap &table,
    const Schema &schema,
    const Expression &predicate)
    : table_(table),
      schema_(schema),
      predicate_(predicate)
{
}

void SeqScanExecutor::init()
{
    rids_ = table_.scan();
    next_rid_index_ = 0;
}

bool SeqScanExecutor::next(Tuple &tuple, RID &rid)
{
    while (next_rid_index_ < rids_.size())
    {
        RID current_rid = rids_[next_rid_index_++];

        Tuple current_tuple;

        if (!table_.get_tuple(current_rid, current_tuple))
        {
            continue;
        }

        if (predicate_.evaluate(current_tuple, schema_))
        {
            tuple = current_tuple;
            rid = current_rid;

            return true;
        }
    }

    return false;
}