// ================================
// File: include/db/page.h
// Fixed-size page abstraction (disk + memory unit)
// ================================
#pragma once

#include <array>
#include "types.h"

namespace db
{

    constexpr size_t PAGE_SIZE = 4096;

    class Page
    {
    public:
        Page(PageId id);

        PageId getId() const;

        void setPageId(PageId id);

        // Raw byte access for serializers / B-Tree nodes
        uint8_t *data();
        const uint8_t *data() const;

        template <typename T>
        T read(size_t offset) const
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
        void write(size_t offset, const T &value)
        {
            if (offset + sizeof(T) > PAGE_SIZE)
            {
                throw std::out_of_range("Reading past the DB page");
            }
            std::memcpy(buffer_.data() + offset, &value, sizeof(T));
            mark_dirty();
        }

        bool is_dirty() const;

        void mark_dirty();

        void clear_dirty();

        // Used to track how many processes are pinning this page, so we do not evict from page table
        int pin_count() const;

        void pin();

        void unpin();

        void reset();

    private:
        PageId page_id_;
        std::array<uint8_t, PAGE_SIZE> buffer_;
        bool is_dirty_;
        int pin_count_;
    };

} // namespace db