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

    private:
        PageId page_id_;
        std::array<uint8_t, PAGE_SIZE> buffer_;
    };

} // namespace db