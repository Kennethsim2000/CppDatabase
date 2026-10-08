#include <gtest/gtest.h>

#include "compound_predicate.h"
#include "predicate.h"
#include "schema.h"
#include "tuple.h"
#include "value.h"

using namespace db;

TEST(CompoundPredicateTest, And)
{
    Schema schema({Column("age", TypeId::INTEGER),
                   Column("active", TypeId::BOOLEAN)});

    Tuple tuple({Value::Integer(30),
                 Value::Boolean(true)},
                schema);

    Predicate age_predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    Predicate active_predicate(
        "active",
        ComparisonType::EQUAL,
        Value::Boolean(true));

    CompoundPredicate predicate(
        age_predicate,
        LogicalOperator::AND,
        active_predicate);

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}

TEST(CompoundPredicateTest, AndReturnsFalse)
{
    Schema schema({Column("age", TypeId::INTEGER),
                   Column("active", TypeId::BOOLEAN)});

    Tuple tuple({Value::Integer(20),
                 Value::Boolean(true)},
                schema);

    Predicate age_predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    Predicate active_predicate(
        "active",
        ComparisonType::EQUAL,
        Value::Boolean(true));

    CompoundPredicate predicate(
        age_predicate,
        LogicalOperator::AND,
        active_predicate);

    EXPECT_FALSE(predicate.evaluate(tuple, schema));
}

TEST(CompoundPredicateTest, Or)
{
    Schema schema({Column("age", TypeId::INTEGER),
                   Column("active", TypeId::BOOLEAN)});

    Tuple tuple({Value::Integer(20),
                 Value::Boolean(true)},
                schema);

    Predicate age_predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    Predicate active_predicate(
        "active",
        ComparisonType::EQUAL,
        Value::Boolean(true));

    CompoundPredicate predicate(
        age_predicate,
        LogicalOperator::OR,
        active_predicate);

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}