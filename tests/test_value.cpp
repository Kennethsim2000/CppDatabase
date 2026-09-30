#include <gtest/gtest.h>

#include "value.h"
#include "column.h"
#include "schema.h"

using namespace db;

// ================================
// Value Tests
// ================================

TEST(ValueTest, CreateInteger)
{
    Value value = Value::Integer(42);

    EXPECT_EQ(value.type(), TypeId::INTEGER);
    EXPECT_EQ(value.as_int(), 42);
}

TEST(ValueTest, CreateVarchar)
{
    Value value = Value::Varchar("Kenneth");

    EXPECT_EQ(value.type(), TypeId::VARCHAR);
    EXPECT_EQ(value.as_string(), "Kenneth");
}

TEST(ValueTest, CreateEmptyVarchar)
{
    Value value = Value::Varchar("");

    EXPECT_EQ(value.type(), TypeId::VARCHAR);
    EXPECT_EQ(value.as_string(), "");
}

TEST(ValueTest, CompareIntegerEqual)
{
    Value left = Value::Integer(42);
    Value right = Value::Integer(42);

    EXPECT_TRUE(
        left.compare(ComparisonType::EQUAL, right));
}

TEST(ValueTest, CompareIntegerNotEqual)
{
    Value left = Value::Integer(42);
    Value right = Value::Integer(20);

    EXPECT_TRUE(
        left.compare(ComparisonType::NOT_EQUAL, right));
}

TEST(ValueTest, CompareIntegerLessThan)
{
    Value left = Value::Integer(20);
    Value right = Value::Integer(42);

    EXPECT_TRUE(
        left.compare(ComparisonType::LESS_THAN, right));

    EXPECT_FALSE(
        left.compare(ComparisonType::GREATER_THAN, right));
}

TEST(ValueTest, CompareIntegerGreaterThan)
{
    Value left = Value::Integer(42);
    Value right = Value::Integer(20);

    EXPECT_TRUE(
        left.compare(ComparisonType::GREATER_THAN, right));

    EXPECT_FALSE(
        left.compare(ComparisonType::LESS_THAN, right));
}

TEST(ValueTest, CompareBigInt)
{
    Value left = Value::BigInt(123456789);
    Value right = Value::BigInt(123456789);

    EXPECT_TRUE(
        left.compare(ComparisonType::EQUAL, right));

    Value larger = Value::BigInt(999999999);

    EXPECT_TRUE(
        larger.compare(
            ComparisonType::GREATER_THAN,
            right));
}

TEST(ValueTest, CompareVarcharEqual)
{
    Value left = Value::Varchar("Kenneth");
    Value right = Value::Varchar("Kenneth");

    EXPECT_TRUE(
        left.compare(ComparisonType::EQUAL, right));
}

TEST(ValueTest, CompareVarcharNotEqual)
{
    Value left = Value::Varchar("Kenneth");
    Value right = Value::Varchar("Alice");

    EXPECT_TRUE(
        left.compare(ComparisonType::NOT_EQUAL, right));
}

TEST(ValueTest, CompareVarcharOrdering)
{
    Value left = Value::Varchar("Alice");
    Value right = Value::Varchar("Kenneth");

    EXPECT_TRUE(
        left.compare(ComparisonType::LESS_THAN, right));

    EXPECT_TRUE(
        right.compare(ComparisonType::GREATER_THAN, left));
}

TEST(ValueTest, CompareBoolean)
{
    Value true_value = Value::Boolean(true);
    Value false_value = Value::Boolean(false);

    EXPECT_TRUE(
        true_value.compare(
            ComparisonType::EQUAL,
            true_value));

    EXPECT_TRUE(
        true_value.compare(
            ComparisonType::NOT_EQUAL,
            false_value));
}

TEST(ValueTest, BooleanOrderingThrows)
{
    Value true_value = Value::Boolean(true);
    Value false_value = Value::Boolean(false);

    EXPECT_THROW(
        true_value.compare(
            ComparisonType::GREATER_THAN,
            false_value),
        std::invalid_argument);
}

TEST(ValueTest, CompareDifferentTypesThrows)
{
    Value integer = Value::Integer(42);
    Value string = Value::Varchar("42");

    EXPECT_THROW(
        integer.compare(
            ComparisonType::EQUAL,
            string),
        std::invalid_argument);
}
// ================================
// Column Tests
// ================================

TEST(ColumnTest, CreateIntegerColumn)
{
    Column column("id", TypeId::INTEGER);

    EXPECT_EQ(column.name(), "id");
    EXPECT_EQ(column.type(), TypeId::INTEGER);

    Column column2("name", TypeId::VARCHAR);

    EXPECT_EQ(column2.name(), "name");
    EXPECT_EQ(column2.type(), TypeId::VARCHAR);
}

// ================================
// Schema Tests
// ================================

TEST(SchemaTest, CreateSchema)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    EXPECT_EQ(schema.column_count(), 3);
}

TEST(SchemaTest, AccessColumns)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    EXPECT_EQ(schema.column(0).name(), "id");
    EXPECT_EQ(schema.column(0).type(), TypeId::INTEGER);

    EXPECT_EQ(schema.column(1).name(), "name");
    EXPECT_EQ(schema.column(1).type(), TypeId::VARCHAR);

    EXPECT_EQ(schema.column(2).name(), "age");
    EXPECT_EQ(schema.column(2).type(), TypeId::INTEGER);
}

TEST(SchemaTest, EmptySchema)
{
    Schema schema({});

    EXPECT_EQ(schema.column_count(), 0);
}

TEST(SchemaTest, CreateUsersSchema)
{
    Schema users({Column("id", TypeId::INTEGER),
                  Column("name", TypeId::VARCHAR),
                  Column("age", TypeId::INTEGER)});

    ASSERT_EQ(users.column_count(), 3);

    const Column &id = users.column(0);
    const Column &name = users.column(1);
    const Column &age = users.column(2);

    EXPECT_EQ(id.name(), "id");
    EXPECT_EQ(id.type(), TypeId::INTEGER);

    EXPECT_EQ(name.name(), "name");
    EXPECT_EQ(name.type(), TypeId::VARCHAR);

    EXPECT_EQ(age.name(), "age");
    EXPECT_EQ(age.type(), TypeId::INTEGER);
}

TEST(SchemaTest, ColumnIndex)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("active", TypeId::BOOLEAN),
                   Column("balance", TypeId::BIGINT)});

    EXPECT_EQ(schema.column_index("id"), 0);
    EXPECT_EQ(schema.column_index("name"), 1);
    EXPECT_EQ(schema.column_index("active"), 2);
    EXPECT_EQ(schema.column_index("balance"), 3);
}

TEST(SchemaTest, ColumnIndexReturnsMinusOneForUnknownColumn)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    EXPECT_EQ(
        schema.column_index("does_not_exist"),
        -1);
}