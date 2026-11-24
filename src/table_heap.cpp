#include "table_heap.h"

#include <algorithm>
#include "index/b_plus_tree.h"

using namespace db;

TableHeap::TableHeap(BufferPoolManager &bpm) : bpm_(bpm) {}

bool TableHeap::add_index(BPlusTree &index, const Schema &schema, std::size_t column_index)
{
    if (column_index >= schema.column_count() ||
        schema.column(column_index).type() != TypeId::INTEGER ||
        !index.empty())
    {
        return false;
    }

    const auto existing = std::find_if(
        indexes_.begin(), indexes_.end(),
        [&index](const IndexInfo &info) { return info.index == &index; });

    if (existing != indexes_.end())
    {
        return false;
    }

    // Backfill tuples inserted before the index was registered.
    for (const RID &rid : scan())
    {
        Tuple tuple;
        if (!get_tuple(rid, tuple))
        {
            return false;
        }

        index.insert(tuple.get_value(schema, column_index).as_int(), rid);
    }

    indexes_.push_back(IndexInfo{&index, schema, column_index});
    return true;
}

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

            for (const IndexInfo &info : indexes_)
            {
                const int key = tuple.get_value(info.schema, info.column_index).as_int();
                info.index->insert(key, rid);
            }

            return rid;
        }
        bpm_.unpin_page(pid);
    }

    // 2. No space --> allocate new page
    PageId new_pid = bpm_.fetch_next_page();
    Page *page = bpm_.fetch_page(new_pid);

    if (page == nullptr)
    {
        return RID();
    }

    TablePage tp(page);
    tp.init();

    if (!tp.insert_tuple(tuple, rid))
    {
        bpm_.unpin_page(new_pid);
        return RID();
    }

    pages.push_back(new_pid);
    bpm_.unpin_page(new_pid);

    for (const IndexInfo &info : indexes_)
    {
        const int key = tuple.get_value(info.schema, info.column_index).as_int();
        info.index->insert(key, rid);
    }

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

std::vector<RID> TableHeap::scan()
{
    std::vector<RID> rids;

    for (PageId pid : pages)
    {
        Page *page = bpm_.fetch_page(pid);

        if (page == nullptr)
        {
            continue;
        }

        TablePage tp(page);
        uint16_t count = tp.tuple_count();

        for (uint16_t slot = 0; slot < count; slot++)
        {
            rids.emplace_back(pid, slot);
        }

        bpm_.unpin_page(pid);
    }

    return rids;
}