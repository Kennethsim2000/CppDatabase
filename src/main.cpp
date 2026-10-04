// ================================
// File: src/main.cpp
// Entry point – wire everything together
// ================================
#include <iostream>
#include <vector>

#include "buffer_pool.h"
#include "disk_manager.h"
#include "predicate.h"
#include "schema.h"
#include "seq_scan_executor.h"
#include "table_heap.h"
#include "tuple.h"
#include "value.h"

using namespace db;

int main()
{
    // TODO:
    // 1. Initialize DiskManager
    // 2. Initialize BufferPoolManager
    // 3. Create BTree index
    // 4. Start transactions
    // 5. Insert / query data
    DiskManager disk("database.db");
    BufferPoolManager bpm(disk, 10);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    TableHeap table(bpm);

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

    table.insert_tuple(
        Tuple({Value::Integer(4),
               Value::Varchar("John"),
               Value::Integer(35)},
              schema));

    Predicate predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    SeqScanExecutor executor(
        table,
        schema,
        predicate);

    std::vector<Tuple> results = executor.execute();

    std::cout << "Users with age > 25:\n";

    for (const Tuple &tuple : results)
    {
        int id = tuple.get_value(schema, 0).as_int();
        std::string name = tuple.get_value(schema, 1).as_string();
        int age = tuple.get_value(schema, 2).as_int();

        std::cout
            << "id=" << id
            << ", name=" << name
            << ", age=" << age
            << '\n';
    }

    return 0;
}