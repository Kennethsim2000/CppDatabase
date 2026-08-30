#include <gtest/gtest.h>

#include "value.h"

using namespace db;

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