#include <iostream>
#include <vector>

#include "buffer_pool.h"
#include "compound_predicate.h"
#include "disk_manager.h"
#include "predicate.h"
#include "projection.h"
#include "schema.h"
#include "seq_scan_executor.h"
#include "table_heap.h"
#include "tuple.h"
#include "value.h"

using namespace db;

int main()
{
    DiskManager disk("database.db");
    BufferPoolManager bpm(disk, 10);

    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER),
                   Column("active", TypeId::BOOLEAN)});

    TableHeap table(bpm);

    table.insert_tuple(Tuple({Value::Integer(1),
                              Value::Varchar("Kenneth"),
                              Value::Integer(24),
                              Value::Boolean(true)},
                             schema));

    table.insert_tuple(Tuple({Value::Integer(2),
                              Value::Varchar("Alice"),
                              Value::Integer(30),
                              Value::Boolean(true)},
                             schema));

    table.insert_tuple(Tuple({Value::Integer(3),
                              Value::Varchar("Bob"),
                              Value::Integer(20),
                              Value::Boolean(true)},
                             schema));

    table.insert_tuple(Tuple({Value::Integer(4),
                              Value::Varchar("John"),
                              Value::Integer(35),
                              Value::Boolean(false)},
                             schema));

    table.insert_tuple(Tuple({Value::Integer(5),
                              Value::Varchar("Emily"),
                              Value::Integer(28),
                              Value::Boolean(true)},
                             schema));

    Predicate age_predicate(
        "age",
        ComparisonType::GREATER_THAN,
        Value::Integer(25));

    Predicate active_predicate(
        "active",
        ComparisonType::EQUAL,
        Value::Boolean(true));

    CompoundPredicate combined_predicate(
        age_predicate,
        LogicalOperator::AND,
        active_predicate);

    SeqScanExecutor executor(
        table,
        schema,
        combined_predicate);

    std::vector<Tuple> results = executor.execute();

    Projection projection(
        schema,
        {"name", "age"});

    std::cout << "Active users with age > 25:\n";

    for (const Tuple &tuple : results)
    {
        Tuple projected_tuple = projection.project(tuple);

        std::string name = projected_tuple
                               .get_value(projection.output_schema(), 0)
                               .as_string();

        int age = projected_tuple
                      .get_value(projection.output_schema(), 1)
                      .as_int();

        std::cout
            << "name=" << name
            << ", age=" << age
            << '\n';
    }

    return 0;
}
