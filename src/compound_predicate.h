#pragma once

#include "predicate.h"

namespace db
{
    enum class LogicalOperator
    {
        AND,
        OR
    };

    class CompoundPredicate
    {
    public:
        CompoundPredicate(
            const Predicate &left,
            LogicalOperator op,
            const Predicate &right);

        bool evaluate(
            const Tuple &tuple,
            const Schema &schema) const;

    private:
        const Predicate &left_;
        LogicalOperator op_;
        const Predicate &right_;
    };
}