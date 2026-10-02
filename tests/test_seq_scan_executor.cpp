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

    SeqScanExecutor executor(
        table,
        schema,
        predicate);

    std::vector<Tuple> results = executor.execute();

    EXPECT_TRUE(results.empty());
}