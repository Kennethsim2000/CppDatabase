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

        PageId id() const;

        // Raw byte access for serializers / B-Tree nodes
        uint8_t *data();
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

        void unpin();

    private:
        PageId page_id;
        std::array<uint8_t, PAGE_SIZE> buffer_;
        bool is_dirty;
        int pin_count;
    };

} // namespace db