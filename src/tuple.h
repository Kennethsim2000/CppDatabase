#pragma once

#include <vector>
#include <cstddef>

#include "value.h"
#include "schema.h"

// Represents one row

namespace db
{
    class Tuple
    {
    public:
        Tuple() = default;

        // Construct a tuple by serializing typed values
        // according to the schema.
        Tuple(
            const std::vector<Value> &values,
            const Schema &schema);

        // Construct a tuple from already serialized bytes.
        explicit Tuple(const std::vector<char> &data)
            : data_(data)
        {
        }

        const char *data() const
        {
            return data_.data();
        }

        size_t size() const
        {
            return data_.size();
        }

        // Deserialize the value at a given column.
        Value get_value(
            const Schema &schema,
            size_t index) const;

    private:
        std::vector<char> data_;

        void append_bytes(
            const void *data,
            size_t size);
    };
}