
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
        executed_ = false;
    }

    bool IndexScanExecutor::next(Tuple &tuple, RID &rid)
    {
        if (executed_)
        {
            return false;
        }

        executed_ = true;

        if (!index_.search(key_, rid))
        {
            return false;
        }

        if (!table_heap_.get_tuple(rid, tuple))
        {
            return false;
        }

        return true;
    }
}
