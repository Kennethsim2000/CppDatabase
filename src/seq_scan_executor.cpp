#include "seq_scan_executor.h"

using namespace db;

SeqScanExecutor::SeqScanExecutor(
    TableHeap &table,
    const Schema &schema,
    const Predicate &predicate) : table_(table), schema_(schema), predicate_(predicate)
{
}

std::vector<Tuple> SeqScanExecutor::execute()
{
    std::vector<Tuple> results;

    std::vector<RID> rids = table_.scan();

    for (const RID &rid : rids)
    {
        Tuple tuple;

        if (!table_.get_tuple(rid, tuple))
        {
            continue;
        }

        if (predicate_.evaluate(tuple, schema_))
        {
            results.push_back(tuple);
        }
    }

    return results;
}