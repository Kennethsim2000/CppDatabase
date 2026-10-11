#include <gtest/gtest.h>

#include "buffer_pool.h"
#include "disk_manager.h"
#include "predicate.h"
#include "projection_executor.h"
#include "schema.h"
#include "seq_scan_executor.h"
#include "table_heap.h"
#include "tuple.h"
#include "value.h"

using namespace db;

TEST(ProjectionExecutorTest, SelectColumns)
{
    DiskManager disk("test_projection_executor.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    table.insert_tuple(
        Tuple({Value::Integer(42),
               Value::Varchar("Kenneth"),
               Value::Integer(24)},
              schema));

    Predicate predicate(
        "id",
        ComparisonType::EQUAL,
        Value::Integer(42));

    SeqScanExecutor scan(table, schema, predicate);

    ProjectionExecutor executor(
        scan,
        schema,
        {"name", "age"});

    executor.init();

    Tuple result;
    RID rid;

    ASSERT_TRUE(executor.next(result, rid));

    const Schema &output_schema = executor.output_schema();

    EXPECT_EQ(
        output_schema.column_index("name"),
        0);

    EXPECT_EQ(
        output_schema.column_index("age"),
        1);

    EXPECT_EQ(
        result.get_value(output_schema, 0).as_string(),
        "Kenneth");

    EXPECT_EQ(
        result.get_value(output_schema, 1).as_int(),
        24);

    EXPECT_FALSE(executor.next(result, rid));
}

TEST(ProjectionExecutorTest, ReturnsMultipleProjectedTuples)
{
    DiskManager disk("test_projection_executor.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    table.insert_tuple(
        Tuple({Value::Integer(1),
               Value::Varchar("Kenneth"),
               Value::Integer(24)},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(2),
               Value::Varchar("Alice"),
               Value::Integer(30)},
              schema));

    table.insert_tuple(
        Tuple({Value::Integer(3),
               Value::Varchar("Bob"),
               Value::Integer(20)},
              schema));

    Predicate predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(21));

    SeqScanExecutor scan(table, schema, predicate);

    ProjectionExecutor executor(
        scan,
        schema,
        {"name"});

    executor.init();

    Tuple result;
    RID rid;
    std::vector<std::string> result_names;

    while (executor.next(result, rid))
    {
        result_names.push_back(
            result.get_value(
                      executor.output_schema(), 0)
                .as_string());
    }

    ASSERT_EQ(result_names.size(), 2);
    EXPECT_EQ(result_names[0], "Kenneth");
    EXPECT_EQ(result_names[1], "Alice");
}

TEST(ProjectionExecutorTest, NoMatchingTuples)
{
    DiskManager disk("test_projection_executor.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    table.insert_tuple(
        Tuple({Value::Integer(1),
               Value::Varchar("Kenneth")},
              schema));

    Predicate predicate(
        "id",
        ComparisonType::GREATER_THAN,
        Value::Integer(100));

    SeqScanExecutor scan(table, schema, predicate);

    ProjectionExecutor executor(
        scan,
        schema,
        {"name"});

    executor.init();

    Tuple result;
    RID rid;

    EXPECT_FALSE(executor.next(result, rid));
}

TEST(ProjectionExecutorTest, EmptyTable)
{
    DiskManager disk("test_projection_executor.db");
    BufferPoolManager bpm(disk, 10);
    TableHeap table(bpm);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    Predicate predicate(
        "id",
        ComparisonType::EQUAL,
        Value::Integer(1));

    SeqScanExecutor scan(table, schema, predicate);

    ProjectionExecutor executor(
        scan,
        schema,
        {"name"});

    executor.init();

    Tuple result;
    RID rid;

    EXPECT_FALSE(executor.next(result, rid));
}