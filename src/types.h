// ================================
// File: include/db/types.h
// Core shared types used across the database
// ================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace db
{

    using PageId = uint64_t;
    using TxnId = uint64_t;

    enum class TypeId
    {
        INTEGER,
        BIGINT,
        BOOLEAN,
        VARCHAR
    };

} // namespace db