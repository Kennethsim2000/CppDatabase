#pragma once

#include "schema.h"
#include "tuple.h"

namespace db
{
    class Expression
    {
    public:
        virtual ~Expression() = default;

        virtual bool evaluate(
            const Tuple &tuple,
            const Schema &schema) const = 0;
    };
}