#include "table_heap.h"

using namespace db;

TableHeap::TableHeap(BufferPoolManager &bpm) : bpm_(bpm) {};

RID TableHeap::insert_tuple(const Tuple &tuple)
{
    RID rid;

    // 1. Try existing pages
    for (PageId pid : pages)
    {
        Page *page = bpm_.fetch_page(pid);
        if (page == nullptr)
            continue;
        TablePage tp(page);

        if (tp.has_space(tuple.size()))
        {
            tp.insert_tuple(tuple, rid);
            bpm_.unpin_page(pid);
            return rid;
        }
        bpm_.unpin_page(pid);
    }

    // 2. No space --> allocate new page
    PageId new_pid = bpm_.fetch_next_page();
    Page *page = bpm_.fetch_page(new_pid);

    TablePage tp(page);
    tp.init();

    tp.insert_tuple(tuple, rid);
    pages.push_back(new_pid);
    bpm_.unpin_page(new_pid);

    return rid;
};

bool TableHeap::get_tuple(const RID &rid, Tuple &tuple)
{
    Page *page = bpm_.fetch_page(rid.page_id);

    if (page == nullptr)
    {
        return false;
    }

    TablePage tp(page);

    bool success = tp.get_tuple(rid, tuple);

    bpm_.unpin_page(rid.page_id);

    return success;
};