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