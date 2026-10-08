#include "compound_predicate.h"

using namespace db;

CompoundPredicate::CompoundPredicate(
    const Predicate &left,
    LogicalOperator op,
    const Predicate &right)
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
    bool right_result = right_.evaluate(tuple, schema);

    switch (op_)
    {
    case LogicalOperator::AND:
        return left_result && right_result;

    case LogicalOperator::OR:
        return left_result || right_result;
    }

    return false;
}