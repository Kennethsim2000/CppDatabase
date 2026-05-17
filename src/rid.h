#pragma once

#include "types.h"
#include "page.h"

namespace db
{

    struct RID
    {
        PageId page_id;
        uint16_t slot_num;

        RID() : page_id(INVALID_PAGE_ID), slot_num(0) {}

        RID(PageId pid, uint16_t slot) : page_id(pid), slot_num(slot) {}
    };

}

// This is used to store the Record ID, every row in a DB needs an address.