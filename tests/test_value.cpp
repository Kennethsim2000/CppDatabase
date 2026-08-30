#include <gtest/gtest.h>

#include "value.h"

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

// ================================
// Column Tests
// ================================

TEST(ColumnTest, CreateIntegerColumn)
{
    Column column("id", TypeId::INTEGER);

    EXPECT_EQ(column.name(), "id");
    EXPECT_EQ(column.type(), TypeId::INTEGER);

    Column column("name", TypeId::VARCHAR);

    EXPECT_EQ(column.name(), "name");
    EXPECT_EQ(column.type(), TypeId::VARCHAR);
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