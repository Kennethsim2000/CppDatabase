#include <gtest/gtest.h>

#include "column.h"
#include "schema.h"
#include "predicate.h"
#include "tuple.h"

using namespace db;

TEST(PredicateTest, EqualString)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    Predicate predicate(
        "name",
        ComparisonType::EQUAL,
        Value::Varchar("Kenneth"));

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, EqualStringReturnsFalse)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    Predicate predicate(
        "name",
        ComparisonType::EQUAL,
        Value::Varchar("Alice"));

    EXPECT_FALSE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, GreaterThanInteger)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    Predicate predicate(
        "id",
        ComparisonType::GREATER_THAN,
        Value::Integer(20));

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, GreaterThanIntegerReturnsFalse)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    Predicate predicate(
        "id",
        ComparisonType::GREATER_THAN,
        Value::Integer(50));

    EXPECT_FALSE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, LessThanOrEqualInteger)
{
    Schema schema({Column("id", TypeId::INTEGER)});

    Tuple tuple({Value::Integer(42)}, schema);

    Predicate predicate(
        "id",
        ComparisonType::LESS_THAN_OR_EQUAL,
        Value::Integer(42));

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, EqualBigInt)
{
    Schema schema({Column("balance", TypeId::BIGINT)});

    Tuple tuple({Value::BigInt(123456789)}, schema);

    Predicate predicate(
        "balance",
        ComparisonType::EQUAL,
        Value::BigInt(123456789));

    EXPECT_TRUE(predicate.evaluate(tuple, schema));
}

TEST(PredicateTest, InvalidColumn)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    Predicate predicate(
        "does_not_exist",
        ComparisonType::EQUAL,
        Value::Integer(42));

    EXPECT_FALSE(predicate.evaluate(tuple, schema));
}