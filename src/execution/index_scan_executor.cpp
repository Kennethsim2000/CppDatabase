#include "execution/index_scan_executor.h"

namespace db
{

    IndexScanExecutor::IndexScanExecutor(
        TableHeap &table_heap,
        const BPlusTree &index,
        int key)
        : table_heap_(table_heap),
          index_(index),
          key_(key)
    {
    }

    void IndexScanExecutor::init()
    {
        rids_.clear();
        index_.search_all(key_, rids_);
        next_rid_index_ = 0;
    }

    bool IndexScanExecutor::next(Tuple &tuple, RID &rid)
    {
        while (next_rid_index_ < rids_.size())
        {
            RID current_rid = rids_[next_rid_index_++];

            Tuple current_tuple;

            if (!table_heap_.get_tuple(current_rid, current_tuple))
            {
                continue;
            }

            tuple = current_tuple;
            rid = current_rid;
            return true;
        }

        return false;
    }
}
