#include "page.h"
#include "types.h"

using namespace db;

Page::Page(PageId id) : page_id_(id), is_dirty_(false), pin_count_(0) {}

PageId Page::getId() const
{
    return page_id_;
}

uint8_t *Page::data()
{
    return buffer_.data();
}

const uint8_t *Page::data() const
{
    return buffer_.data();
}

bool Page::is_dirty() const
{
    return is_dirty_;
}

void Page::mark_dirty()
{
    is_dirty_ = true;
}

void Page::clear_dirty()
{
    is_dirty_ = false;
}

int Page::pin_count() const
{
    return pin_count_;
}

void Page::pin()
{
    pin_count_++;
}

void Page::unpin()
{
    if (pin_count_ > 0)
        pin_count_--;
}

void Page::reset()
{
    page_id_ = -1; // reset to INVALID PAGE_ID
    std::memset(buffer_.data(), 0, PAGE_SIZE);
    is_dirty_ = false;
    pin_count_ = 0;
}