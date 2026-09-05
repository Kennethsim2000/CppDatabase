#include <gtest/gtest.h>

#include "tuple.h"
#include "schema.h"
#include "value.h"

using namespace db;

// ================================
// Tuple Tests
// ================================

TEST(TupleTest, EmptyTuple)
{
    Tuple tuple;

    EXPECT_EQ(tuple.size(), 0);
}

TEST(TupleTest, SerializeInteger)
{
    Schema schema({Column("id", TypeId::INTEGER)});

    Tuple tuple({Value::Integer(42)}, schema);

    EXPECT_GT(tuple.size(), 0);

    Value value = tuple.get_value(schema, 0);

    EXPECT_EQ(value.type(), TypeId::INTEGER);
    EXPECT_EQ(value.as_int(), 42);
}

TEST(TupleTest, SerializeBigInt)
{
    Schema schema({Column("id", TypeId::BIGINT)});

    Tuple tuple({Value::BigInt(123456789)}, schema);

    Value value = tuple.get_value(schema, 0);

    EXPECT_EQ(value.type(), TypeId::BIGINT);
    EXPECT_EQ(value.as_bigint(), 123456789);
}

TEST(TupleTest, SerializeBoolean)
{
    Schema schema({Column("active", TypeId::BOOLEAN)});

    Tuple tuple({Value::Boolean(true)}, schema);

    Value value = tuple.get_value(schema, 0);

    EXPECT_EQ(value.type(), TypeId::BOOLEAN);
    EXPECT_TRUE(value.as_bool());
}

TEST(TupleTest, SerializeVarchar)
{
    Schema schema({Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Varchar("Kenneth")}, schema);

    Value value = tuple.get_value(schema, 0);

    EXPECT_EQ(value.type(), TypeId::VARCHAR);
    EXPECT_EQ(value.as_string(), "Kenneth");
}

TEST(TupleTest, SerializeMultipleTypes)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("active", TypeId::BOOLEAN),
                   Column("balance", TypeId::BIGINT)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth"),
                 Value::Boolean(true),
                 Value::BigInt(123456789)},
                schema);

    ASSERT_GT(tuple.size(), 0);

    Value id = tuple.get_value(schema, 0);
    Value name = tuple.get_value(schema, 1);
    Value active = tuple.get_value(schema, 2);
    Value balance = tuple.get_value(schema, 3);

    EXPECT_EQ(id.type(), TypeId::INTEGER);
    EXPECT_EQ(id.as_int(), 42);

    EXPECT_EQ(name.type(), TypeId::VARCHAR);
    EXPECT_EQ(name.as_string(), "Kenneth");

    EXPECT_EQ(active.type(), TypeId::BOOLEAN);
    EXPECT_TRUE(active.as_bool());

    EXPECT_EQ(balance.type(), TypeId::BIGINT);
    EXPECT_EQ(balance.as_bigint(), 123456789);
}

TEST(TupleTest, SerializeMultipleVarchars)
{
    Schema schema({Column("first_name", TypeId::VARCHAR),
                   Column("last_name", TypeId::VARCHAR),
                   Column("description", TypeId::VARCHAR)});

    Tuple tuple({Value::Varchar("Kenneth"),
                 Value::Varchar("Sim"),
                 Value::Varchar("Software Engineer")},
                schema);

    EXPECT_EQ(
        tuple.get_value(schema, 0).as_string(),
        "Kenneth");

    EXPECT_EQ(
        tuple.get_value(schema, 1).as_string(),
        "Sim");

    EXPECT_EQ(
        tuple.get_value(schema, 2).as_string(),
        "Software Engineer");
}

TEST(TupleTest, RejectsWrongNumberOfValues)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    EXPECT_THROW(
        Tuple({Value::Integer(42)}, schema),
        std::invalid_argument);
}

TEST(TupleTest, RejectsWrongValueType)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    EXPECT_THROW(
        Tuple({Value::Varchar("Kenneth"),
               Value::Integer(42)},
              schema),
        std::invalid_argument);
}

TEST(TupleTest, RejectsInvalidColumnIndex)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth")},
                schema);

    EXPECT_THROW(
        tuple.get_value(schema, 2),
        std::out_of_range);
}