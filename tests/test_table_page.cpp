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
// Tuple Tests
// ================================

TEST(TupleTest, EmptyTuple)
{
    Tuple tuple;

    EXPECT_EQ(tuple.size(), 0);
}

TEST(TupleTest, StoresData)
{
    std::vector<char> data = {'h', 'e', 'l', 'l', 'o'};

    Tuple tuple(data);

    EXPECT_EQ(tuple.size(), 5);
    EXPECT_EQ(std::memcmp(tuple.data(), data.data(), data.size()), 0);
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

TEST_F(TablePageTest, InsertTuple)
{
    std::vector<char> data = {'h', 'e', 'l', 'l', 'o'};
    Tuple tuple(data);
    RID rid;

    EXPECT_TRUE(table_page.insert_tuple(tuple, rid));

    EXPECT_EQ(rid.page_id, 1);
    EXPECT_EQ(rid.slot_num, 0);

    EXPECT_TRUE(page.is_dirty());
}

TEST_F(TablePageTest, InsertAndRetrieveTuple)
{
    std::vector<char> data = {'h', 'e', 'l', 'l', 'o'};
    Tuple tuple(data);

    RID rid;
    ASSERT_TRUE(table_page.insert_tuple(tuple, rid));

    Tuple result;
    ASSERT_TRUE(table_page.get_tuple(rid, result));
    EXPECT_EQ(result.size(), tuple.size());
    EXPECT_EQ(
        std::memcmp(
            result.data(),
            tuple.data(),
            tuple.size()),
        0);
}

TEST_F(TablePageTest, RetrieveMultipleTuples)
{
    Tuple tuple1(std::vector<char>{'a', 'b', 'c'});
    Tuple tuple2(std::vector<char>{'d', 'e', 'f', 'g'});
    Tuple tuple3(std::vector<char>{'h', 'i'});

    RID rid1;
    RID rid2;
    RID rid3;

    ASSERT_TRUE(table_page.insert_tuple(tuple1, rid1));
    ASSERT_TRUE(table_page.insert_tuple(tuple2, rid2));
    ASSERT_TRUE(table_page.insert_tuple(tuple3, rid3));

    Tuple result1;
    Tuple result2;
    Tuple result3;

    ASSERT_TRUE(table_page.get_tuple(rid1, result1));
    ASSERT_TRUE(table_page.get_tuple(rid2, result2));
    ASSERT_TRUE(table_page.get_tuple(rid3, result3));

    EXPECT_EQ(result1.size(), 3);
    EXPECT_EQ(result2.size(), 4);
    EXPECT_EQ(result3.size(), 2);

    EXPECT_EQ(
        std::memcmp(result1.data(), tuple1.data(), tuple1.size()),
        0);

    EXPECT_EQ(
        std::memcmp(result2.data(), tuple2.data(), tuple2.size()),
        0);

    EXPECT_EQ(
        std::memcmp(result3.data(), tuple3.data(), tuple3.size()),
        0);
}

TEST_F(TablePageTest, InvalidSlotReturnsFalse)
{
    Tuple tuple(std::vector<char>{'h', 'e', 'l', 'l', 'o'});

    RID rid;
    ASSERT_TRUE(table_page.insert_tuple(tuple, rid));

    RID invalid_rid(1, 100);
    Tuple result;
    EXPECT_FALSE(
        table_page.get_tuple(invalid_rid, result));
}

TEST_F(TablePageTest, RejectsTupleWhenPageIsFull)
{
    // Fill almost the entire page.
    Tuple large_tuple(
        std::vector<char>(
            PAGE_SIZE - sizeof(TablePageHeader) - sizeof(Slot),
            'x'));

    RID rid;
    ASSERT_TRUE(
        table_page.insert_tuple(large_tuple, rid));

    // There should not be enough space for another tuple.
    Tuple another_tuple(
        std::vector<char>{'a'});

    RID another_rid;
    EXPECT_FALSE(
        table_page.insert_tuple(
            another_tuple,
            another_rid));
}

TEST_F(TablePageTest, VariableSizedTuples)
{
    Tuple small(std::vector<char>{'a'});
    Tuple medium(std::vector<char>(100, 'b'));
    Tuple large(std::vector<char>(500, 'c'));

    RID small_rid;
    RID medium_rid;
    RID large_rid;

    ASSERT_TRUE(table_page.insert_tuple(small, small_rid));
    ASSERT_TRUE(table_page.insert_tuple(medium, medium_rid));
    ASSERT_TRUE(table_page.insert_tuple(large, large_rid));

    Tuple result_small;
    Tuple result_medium;
    Tuple result_large;

    ASSERT_TRUE(
        table_page.get_tuple(small_rid, result_small));
    ASSERT_TRUE(
        table_page.get_tuple(medium_rid, result_medium));
    ASSERT_TRUE(
        table_page.get_tuple(large_rid, result_large));

    EXPECT_EQ(result_small.size(), 1);
    EXPECT_EQ(result_medium.size(), 100);
    EXPECT_EQ(result_large.size(), 500);

    EXPECT_EQ(
        std::memcmp(
            result_small.data(),
            small.data(),
            small.size()),
        0);

    EXPECT_EQ(
        std::memcmp(
            result_medium.data(),
            medium.data(),
            medium.size()),
        0);

    EXPECT_EQ(
        std::memcmp(
            result_large.data(),
            large.data(),
            large.size()),
        0);
}