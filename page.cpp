#include "page.h"
#include "types.h"

using namespace db;

Page::Page(PageId id) : page_id_(id) {}

PageId Page::id() const
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

template <typename T>
T Page::read(size_t offset) const
{
    if (offset + sizeof(T) > PAGE_SIZE)
    {
        throw std::out_of_range("Reading past the DB page");
    }
    T val;
    std::memcpy(&val, buffer_.data() + offset, sizeof(T));
    return val;
}

template <typename T>
void Page::write(size_t offset, T &value)
{
    if (offset + sizeof(T) > PAGE_SIZE)
    {
        throw std::out_of_range("Reading past the DB page");
    }
    std::memcpy(buffer_.data() + offset, &value, sizeof(value));
}

bool Page::is_dirty()
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

int Page::pin_count()
{
    return pin_count_;
}

void Page::pin()
{
    pin_count_++;
}

void Page::unpin()
{
    pin_count_--;
}

/*  uint8_t *data();
        const uint8_t *data() const;

        template <typename T>
        T read(size_t offset) const;

        template <typename T>
        void write(size_t offset, T &value);

        bool is_dirty();

        void mark_dirty();

        void clear_dirty();

        int pin_count;

        void pin();

        void unpin();*/