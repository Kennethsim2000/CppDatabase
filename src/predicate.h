#pragma once

#include <string>

#include "tuple.h"
#include "schema.h"
#include "value.h"

namespace db
{
    enum class ComparisonType
    {
        EQUAL,
        NOT_EQUAL,
        LESS_THAN,
        LESS_THAN_OR_EQUAL,
        GREATER_THAN,
        GREATER_THAN_OR_EQUAL
    };

    class Predicate
    {
    public:
        Predicate(
            const std::string &column_name,
            ComparisonType comparison,
            const Value &value);

        bool evaluate(
            const Tuple &tuple,
            const Schema &schema) const;

    private:
        std::string column_name_;
        ComparisonType comparison_;
        Value value_;
    };
}