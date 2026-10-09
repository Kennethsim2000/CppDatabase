#include "compound_predicate.h"

using namespace db;

CompoundPredicate::CompoundPredicate(
    const Expression &left,
    LogicalOperator op,
    const Expression &right)
    : left_(left),
      op_(op),
      right_(right)
{
}

bool CompoundPredicate::evaluate(
    const Tuple &tuple,
    const Schema &schema) const
{
    bool left_result = left_.evaluate(tuple, schema);

    if (op_ == LogicalOperator::AND)
    {
        if (!left_result)
        {
            return false;
        }

        return right_.evaluate(tuple, schema);
    }

    if (op_ == LogicalOperator::OR)
    {
        if (left_result)
        {
            return true;
        }

        return right_.evaluate(tuple, schema);
    }

    return false;
}