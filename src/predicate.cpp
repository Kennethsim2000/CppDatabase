#include "predicate.h"

using namespace db;

Predicate::Predicate(
    const std::string &column_name,
    ComparisonType comparison,
    const Value &value)
    : column_name_(column_name),
      comparison_(comparison),
      value_(value)
{
}

bool Predicate::evaluate(
    const Tuple &tuple,
    const Schema &schema) const
{
    int index = schema.column_index(column_name_);

    if (index == -1)
    {
        return false;
    }

    Value tuple_value = tuple.get_value(schema, index);

    return tuple_value.compare(
        comparison_,
        value_);
}