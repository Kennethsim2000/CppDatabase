#include "table_page.h"

using namespace db;

TablePage::TablePage(Page *page) : page_(page)
{
}

TablePageHeader *TablePage::header()
{
    return reinterpret_cast<TablePageHeader *>(page_->data());
}

Slot *TablePage::slots()
{
    return reinterpret_cast<Slot *>(page_->data() + sizeof(TablePageHeader));
}

void TablePage::init()
{
    auto *h = header();
    h->tuple_count = 0;
    h->free_space_ptr = PAGE_SIZE;
}

bool TablePage::has_space(size_t tuple_size)
{
    auto *h = header();
    size_t slot_end = sizeof(TablePageHeader) + h->tuple_count * sizeof(Slot);
    size_t space_left = h->free_space_ptr - slot_end;
    return space_left >= tuple_size + sizeof(Slot);
}

/*
      STEP 1 -- check space
      STEP 2 -- Get header
      STEP 3 -- Move free space pointer backward
      STEP 4 -- copy tuple bytes
      STEP 5 -- Write slot metadata
      STEP 6 -- create RID
      step 7 -- increment tuple count(in header)
      step 8 -- Mark page as dirty
  */
bool TablePage::insert_tuple(const Tuple &tuple, RID &rid)
{
    if (!has_space(tuple.size()))
    {
        return false;
    }

    auto *h = header();

    h->free_space_ptr -= tuple.size();

    uint16_t offset = h->free_space_ptr;

    std::memcpy(page_->data() + offset, tuple.data(), tuple.size());

    Slot *slot_array = slots();

    uint16_t slot_num = h->tuple_count;

    slot_array[slot_num].offset = offset;
    slot_array[slot_num].size = tuple.size();

    rid = RID(page_->getId(), slot_num);

    h->tuple_count++;

    page_->mark_dirty();

    return true;
}

bool TablePage::get_tuple(const RID &rid, Tuple &tuple)
{
    /*
        STEP 1 -- Validate slot num
        STEP 2 -- locate slot
        STEP 3 -- Read tuple bytes
        STEP 4 -- Rebuild tuple
    */
    auto *h = header();

    if (rid.slot_num >= h->tuple_count)
    {
        return false;
    }

    Slot *slot_array = slots();

    Slot &slot = slot_array[rid.slot_num];
    uint16_t offset = slot.offset;
    uint16_t size = slot.size;

    const uint8_t *data = page_->data() + offset;

    std::vector<char> bytes(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data + size));

    tuple = Tuple(bytes);
    return true;
}
