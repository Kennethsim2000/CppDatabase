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

        int column_index(const std::string &name) const
        {
            for (int i = 0; i < columns_.size(); i++)
            {
                if (columns_[i].name() == name)
                {
                    return i;
                }
            }

            return -1;
        }

        void add_column(const Column &column)
        {
            columns_.push_back(column);
        }

    private:
        std::vector<Column> columns_;
    };
}