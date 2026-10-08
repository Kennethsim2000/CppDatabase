#include <gtest/gtest.h>

#include "projection.h"
#include "schema.h"
#include "tuple.h"
#include "value.h"

using namespace db;

TEST(ProjectionTest, SelectColumns)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR),
                   Column("age", TypeId::INTEGER)});

    Tuple tuple({Value::Integer(42),
                 Value::Varchar("Kenneth"),
                 Value::Integer(24)},
                schema);

    Projection projection(
        schema,
        {"name", "age"});

    Tuple result = projection.project(tuple);

    EXPECT_EQ(result.get_value(
                        projection.output_schema(), 0)
                  .as_string(),
              "Kenneth");

    EXPECT_EQ(result.get_value(
                        projection.output_schema(), 1)
                  .as_int(),
              24);
}

TEST(ProjectionTest, InvalidColumnThrows)
{
    Schema schema({Column("id", TypeId::INTEGER),
                   Column("name", TypeId::VARCHAR)});

    EXPECT_THROW(
        Projection projection(
            schema,
            {"does_not_exist"}),
        std::invalid_argument);
}