#include <gtest/gtest.h>

#include "buffer_pool.h"
#include "disk_manager.h"
#include "predicate.h"
#include "schema.h"
#include "seq_scan_executor.h"
#include "table_heap.h"
#include "tuple.h"
#include "value.h"

using namespace db;

TEST(SeqScanExecutorTest, NoMatchingTuples)
{
    DiskManager disk("test_seq_scan.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    table.insert_tuple(
        Tuple({Value::Integer(1),
               Value::Varchar("Kenneth")},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(2),
               Value::Varchar("Alice")},
              schema));

    Predicate predicate(
        "id",
        ComparisonType::GREATER_THAN,
        Value::Integer(100));

    SeqScanExecutor executor(table, schema, predicate);
    executor.init();

    Tuple tuple;
    RID rid;

    EXPECT_FALSE(executor.next(tuple, rid));
}

TEST(SeqScanExecutorTest, ReturnsMatchingTuple)
{
    DiskManager disk("test_seq_scan.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    table.insert_tuple(
        Tuple({Value::Integer(1),
               Value::Varchar("Kenneth")},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(2),
               Value::Varchar("Alice")},
              schema));

    Predicate predicate(
        "id",
        ComparisonType::EQUAL,
        Value::Integer(1));

    SeqScanExecutor executor(table, schema, predicate);
    executor.init();

    Tuple tuple;
    RID rid;

    ASSERT_TRUE(executor.next(tuple, rid));

    EXPECT_EQ(tuple.get_value(schema, 0).as_int(), 1);
    EXPECT_EQ(tuple.get_value(schema, 1).as_string(), "Kenneth");

    EXPECT_FALSE(executor.next(tuple, rid));
}

TEST(SeqScanExecutorTest, ReturnsMultipleMatchingTuples)
{
    DiskManager disk("test_seq_scan.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("age", TypeId::INTEGER)});

    table.insert_tuple(
        Tuple({Value::Integer(1),
               Value::Integer(20)},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(2),
               Value::Integer(30)},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(3),
               Value::Integer(40)},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(4),
               Value::Integer(18)},
              schema));

    Predicate predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    SeqScanExecutor executor(table, schema, predicate);
    executor.init();

    Tuple tuple;
    RID rid;
    std::vector<int> result_ids;

    while (executor.next(tuple, rid))
    {
        result_ids.push_back(
            tuple.get_value(schema, 0).as_int());
    }

    ASSERT_EQ(result_ids.size(), 2);
    EXPECT_EQ(result_ids[0], 2);
    EXPECT_EQ(result_ids[1], 3);
}

TEST(SeqScanExecutorTest, EmptyTable)
{
    DiskManager disk("test_seq_scan.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER)});

    Predicate predicate(
        "id",
        ComparisonType::EQUAL,
        Value::Integer(1));

    SeqScanExecutor executor(table, schema, predicate);
    executor.init();

    Tuple tuple;
    RID rid;

    EXPECT_FALSE(executor.next(tuple, rid));
}
