
#include <gtest/gtest.h>

#include "execution/index_scan_executor.h"
#include "buffer_pool.h"
#include "disk_manager.h"
#include "table_heap.h"
#include "tuple.h"
#include "schema.h"
#include "column.h"
#include "value.h"

using namespace db;

class IndexScanExecutorTest : public ::testing::Test
{
protected:
    DiskManager disk{"test_index_scan.db"};
    BufferPoolManager bpm{disk, 10};
    TableHeap table{bpm};

    Schema schema{
        std::vector<Column>{
            Column("id", TypeId::INTEGER),
            Column("age", TypeId::INTEGER)}};

    BPlusTree index;

    RID insert_user(int id, int age)
    {
        Tuple tuple(
            {Value::Integer(id),
             Value::Integer(age)},
            schema);

        RID rid = table.insert_tuple(tuple);
        EXPECT_TRUE(index.insert(id, rid));

        return rid;
    }
};

TEST_F(IndexScanExecutorTest, FindsMatchingTuple)
{
    insert_user(1, 25);
    insert_user(2, 30);
    insert_user(3, 22);

    IndexScanExecutor executor(table, index, 2);
    executor.init();

    Tuple result;
    RID rid;

    ASSERT_TRUE(executor.next(result, rid));

    EXPECT_EQ(result.get_value(schema, 0).as_int(), 2);
    EXPECT_EQ(result.get_value(schema, 1).as_int(), 30);
}

TEST_F(IndexScanExecutorTest, MissingKeyReturnsFalse)
{
    insert_user(1, 25);
    insert_user(3, 22);

    IndexScanExecutor executor(table, index, 2);
    executor.init();

    Tuple result;
    RID rid;

    EXPECT_FALSE(executor.next(result, rid));
}

TEST_F(IndexScanExecutorTest, ReturnsFalseAfterFirstResult)
{
    insert_user(10, 25);
    insert_user(20, 30);

    IndexScanExecutor executor(table, index, 10);
    executor.init();

    Tuple result;
    RID rid;

    ASSERT_TRUE(executor.next(result, rid));
    EXPECT_EQ(result.get_value(schema, 0).as_int(), 10);

    EXPECT_FALSE(executor.next(result, rid));
}

TEST_F(IndexScanExecutorTest, InitResetsExecutor)
{
    insert_user(42, 28);

    IndexScanExecutor executor(table, index, 42);
    executor.init();

    Tuple result;
    RID rid;

    ASSERT_TRUE(executor.next(result, rid));
    EXPECT_EQ(result.get_value(schema, 0).as_int(), 42);

    EXPECT_FALSE(executor.next(result, rid));

    executor.init();

    ASSERT_TRUE(executor.next(result, rid));
    EXPECT_EQ(result.get_value(schema, 0).as_int(), 42);
}
