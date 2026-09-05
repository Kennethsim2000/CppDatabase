#include <gtest/gtest.h>

#include "page.h"
#include "tuple.h"
#include "rid.h"
#include "table_page.h"

using namespace db;

// ================================
// RID Tests
// ================================

TEST(RIDTest, DefaultConstructor)
{
    RID rid;

    EXPECT_EQ(rid.page_id, INVALID_PAGE_ID);
    EXPECT_EQ(rid.slot_num, 0);
}

TEST(RIDTest, ParameterizedConstructor)
{
    RID rid(42, 5);

    EXPECT_EQ(rid.page_id, 42);
    EXPECT_EQ(rid.slot_num, 5);
}

// ================================
// TablePage Tests
// ================================

class TablePageTest : public ::testing::Test
{
protected:
    Page page;
    TablePage table_page{&page};

    void SetUp() override
    {
        page.setPageId(1);
        table_page.init();
    }
};

TEST_F(TablePageTest, InitializesCorrectly)
{
    EXPECT_EQ(page.getId(), 1);
    EXPECT_EQ(page.pin_count(), 0);
    EXPECT_FALSE(page.is_dirty());

    EXPECT_TRUE(table_page.has_space(100));
}

TEST_F(TablePageTest, InsertAndRetrieveTypedTuple)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("active", TypeId::BOOLEAN)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth"),
                 Value::Boolean(true)},
                schema);

    RID rid;

    ASSERT_TRUE(
        table_page.insert_tuple(tuple, rid));

    EXPECT_EQ(rid.page_id, 1);
    EXPECT_EQ(rid.slot_num, 0);

    Tuple result;

    ASSERT_TRUE(
        table_page.get_tuple(rid, result));

    EXPECT_EQ(
        result.get_value(schema, 0).as_int(),
        42);

    EXPECT_EQ(
        result.get_value(schema, 1).as_string(),
        "Kenneth");

    EXPECT_TRUE(
        result.get_value(schema, 2).as_bool());
}

TEST_F(TablePageTest, InsertAndRetrieveMultipleTypedTuples)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("active", TypeId::BOOLEAN)});

    Tuple tuple1({Value::Integer(1),
                  Value::Varchar("Alice"),
                  Value::Boolean(true)},
                 schema);

    Tuple tuple2({Value::Integer(2),
                  Value::Varchar("Bob"),
                  Value::Boolean(false)},
                 schema);

    Tuple tuple3({Value::Integer(3),
                  Value::Varchar("Christopher"),
                  Value::Boolean(true)},
                 schema);

    RID rid1;
    RID rid2;
    RID rid3;

    ASSERT_TRUE(
        table_page.insert_tuple(tuple1, rid1));

    ASSERT_TRUE(
        table_page.insert_tuple(tuple2, rid2));

    ASSERT_TRUE(
        table_page.insert_tuple(tuple3, rid3));

    Tuple result1;
    Tuple result2;
    Tuple result3;

    ASSERT_TRUE(
        table_page.get_tuple(rid1, result1));

    ASSERT_TRUE(
        table_page.get_tuple(rid2, result2));

    ASSERT_TRUE(
        table_page.get_tuple(rid3, result3));

    EXPECT_EQ(
        result1.get_value(schema, 0).as_int(),
        1);
    EXPECT_EQ(
        result1.get_value(schema, 1).as_string(),
        "Alice");
    EXPECT_TRUE(
        result1.get_value(schema, 2).as_bool());

    EXPECT_EQ(
        result2.get_value(schema, 0).as_int(),
        2);
    EXPECT_EQ(
        result2.get_value(schema, 1).as_string(),
        "Bob");
    EXPECT_FALSE(
        result2.get_value(schema, 2).as_bool());

    EXPECT_EQ(
        result3.get_value(schema, 0).as_int(),
        3);
    EXPECT_EQ(
        result3.get_value(schema, 1).as_string(),
        "Christopher");
    EXPECT_TRUE(
        result3.get_value(schema, 2).as_bool());
}

TEST_F(TablePageTest, InsertAndRetrieveAllTypes)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("timestamp", TypeId::BIGINT),
                   Column("active", TypeId::BOOLEAN),
                   Column("name", TypeId::VARCHAR)});

    Tuple tuple({Value::Integer(42),
                 Value::BigInt(1234567890123),
                 Value::Boolean(true),
                 Value::Varchar("Kenneth")},
                schema);

    RID rid;

    ASSERT_TRUE(
        table_page.insert_tuple(tuple, rid));

    Tuple result;

    ASSERT_TRUE(
        table_page.get_tuple(rid, result));
    EXPECT_EQ(
        result.get_value(schema, 0).as_int(),
        42);
    EXPECT_EQ(
        result.get_value(schema, 1).as_bigint(),
        1234567890123);
    EXPECT_TRUE(
        result.get_value(schema, 2).as_bool());
    EXPECT_EQ(
        result.get_value(schema, 3).as_string(),
        "Kenneth");
}

TEST_F(TablePageTest, StoresRawTupleBytes)
{
    std::vector<char> data = {
        'h', 'e', 'l', 'l', 'o'};

    Tuple tuple(data);

    RID rid;

    ASSERT_TRUE(
        table_page.insert_tuple(tuple, rid));

    Tuple result;

    ASSERT_TRUE(
        table_page.get_tuple(rid, result));

    EXPECT_EQ(result.size(), tuple.size());

    EXPECT_EQ(
        std::memcmp(
            result.data(),
            tuple.data(),
            tuple.size()),
        0);
}