#pragma once
#include <vector>
#include <cstring>

// Represents one row

namespace db
{
    class Tuple
    {
    public:
        Tuple() = default;
        explicit Tuple(const std::vector<char> &data) : data_(std::move(data)) {}

        const char *data() const
        {
            return data_.data();
        }

        size_t size() const
        {
            return data_.size();
        }

    private:
        std::vector<char> data_;
    };
}