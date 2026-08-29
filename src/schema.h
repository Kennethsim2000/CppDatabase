#pragma once

#include "column.h"

namespace db
{

    class Schema
    {
    public:
        explicit Schema(std::vector<Column> columns)
            : columns_(std::move(columns))
        {
        }

        size_t column_count() const
        {
            return columns_.size();
        }

        const Column &column(size_t index) const
        {
            return columns_[index];
        }

    private:
        std::vector<Column> columns_;
    };
}