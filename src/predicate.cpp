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
    return false;
}