#pragma once

#include "expression.h"
#include "predicate.h"

namespace db
{
    enum class LogicalOperator
    {
        AND,
        OR
    };

    class CompoundPredicate : public Expression
    {
    public:
        CompoundPredicate(
            const Expression &left,
            LogicalOperator op,
            const Expression &right);

        bool evaluate(
            const Tuple &tuple,
            const Schema &schema) const override;

    private:
        const Expression &left_;
        LogicalOperator op_;
        const Expression &right_;
    };
}