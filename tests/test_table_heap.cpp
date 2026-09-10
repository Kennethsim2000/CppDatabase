#include <gtest/gtest.h>

#include "buffer_pool.h"
#include "disk_manager.h"
#include "schema.h"
#include "table_heap.h"
#include "tuple.h"
#include "value.h"

using namespace db;

// ================================
// Test Fixture
// ================================

class TableHeapTest : public ::testing::Test
{
protected:
    DiskManager disk_manager{"test.db"};
    BufferPoolManager bpm{disk_manager, 10};
    TableHeap table_heap{bpm};

    Schema schema{
        {Column("id", TypeId::INTEGER),
         Column("name", TypeId::VARCHAR),
         Column("active", TypeId::BOOLEAN),
         Column("balance", TypeId::BIGINT)}};
};

// ================================
// Basic Insert / Retrieve
// ================================

TEST_F(TableHeapTest, InsertAndRetrieveTuple)
{
    Tuple tuple(
        {Value::Integer(42),
         Value::Varchar("Kenneth"),
         Value::Boolean(true),
         Value::BigInt(1000)},
        schema);

    RID rid = table_heap.insert_tuple(tuple);

    EXPECT_NE(rid.page_id, INVALID_PAGE_ID);
    EXPECT_EQ(rid.slot_num, 0);

    Tuple result;

    ASSERT_TRUE(
        table_heap.get_tuple(rid, result));

    EXPECT_EQ(
        result.size(),
        tuple.size());
    EXPECT_EQ(
        result.get_value(schema, 0).as_int(),
        42);
    EXPECT_EQ(
        result.get_value(schema, 1).as_string(),
        "Kenneth");
    EXPECT_EQ(
        result.get_value(schema, 2).as_bool(),
        true);
    EXPECT_EQ(
        result.get_value(schema, 3).as_bigint(),
        1000);
}

// ================================
// Large Number of Tuples
// ================================

TEST_F(TableHeapTest, InsertManyTuples)
{
    constexpr int NUM_TUPLES = 100;

    std::vector<RID> rids;

    for (int i = 0; i < NUM_TUPLES; i++)
    {
        Tuple tuple(
            {Value::Integer(i),
             Value::Varchar("user"),
             Value::Boolean(i % 2 == 0),
             Value::BigInt(i * 100)},
            schema);

        rids.push_back(
            table_heap.insert_tuple(tuple));
    }

    ASSERT_EQ(
        rids.size(),
        NUM_TUPLES);

    for (int i = 0; i < NUM_TUPLES; i++)
    {
        Tuple result;

        ASSERT_TRUE(
            table_heap.get_tuple(
                rids[i],
                result));

        EXPECT_EQ(
            result.get_value(schema, 0).as_int(),
            i);
        EXPECT_EQ(
            result.get_value(schema, 1).as_string(),
            "user");
        EXPECT_EQ(
            result.get_value(schema, 2).as_bool(),
            i % 2 == 0);
        EXPECT_EQ(
            result.get_value(schema, 3).as_bigint(),
            i * 100);
    }
}

// ================================
// Multiple Pages
// ================================

TEST_F(TableHeapTest, InsertsAcrossMultiplePages)
{
    std::vector<RID> rids;

    constexpr int NUM_TUPLES = 100;

    for (int i = 0; i < NUM_TUPLES; i++)
    {
        // Make each tuple reasonably large so that
        // tuples cannot all fit on one page.
        std::string name(
            200,
            'a' + (i % 26));

        Tuple tuple(
            {Value::Integer(i),
             Value::Varchar(name),
             Value::Boolean(true),
             Value::BigInt(i)},
            schema);

        rids.push_back(
            table_heap.insert_tuple(tuple));
    }

    // We should have spilled onto multiple pages.
    bool found_multiple_pages = false;

    for (size_t i = 1; i < rids.size(); i++)
    {
        if (rids[i].page_id != rids[0].page_id)
        {
            found_multiple_pages = true;
            break;
        }
    }

    EXPECT_TRUE(found_multiple_pages);

    // Every tuple should still be retrievable.
    for (int i = 0; i < NUM_TUPLES; i++)
    {
        Tuple result;

        ASSERT_TRUE(
            table_heap.get_tuple(
                rids[i],
                result));

        EXPECT_EQ(
            result.get_value(schema, 0).as_int(),
            i);
    }
}

// ================================
// Invalid RID
// ================================

TEST_F(TableHeapTest, InvalidRIDReturnsFalse)
{
    RID invalid_rid(
        INVALID_PAGE_ID,
        0);

    Tuple result;

    EXPECT_FALSE(
        table_heap.get_tuple(
            invalid_rid,
            result));
}