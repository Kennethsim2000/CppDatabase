#include "projection.h"

#include <stdexcept>

using namespace db;

static Schema build_output_schema(
    const Schema &input_schema,
    const std::vector<std::string> &column_names)
{
    std::vector<Column> columns;

    for (const std::string &name : column_names)
    {
        int index = input_schema.column_index(name);

        if (index == -1)
        {
            throw std::invalid_argument(
                "Column does not exist: " + name);
        }

        columns.push_back(input_schema.column(index));
    }

    return Schema(std::move(columns));
}

Projection::Projection(
    const Schema &input_schema,
    const std::vector<std::string> &column_names)
    : input_schema_(input_schema),
      output_schema_(build_output_schema(
          input_schema,
          column_names))
{
    for (const std::string &name : column_names)
    {
        column_indices_.push_back(
            input_schema.column_index(name));
    }
}

Tuple Projection::project(const Tuple &tuple) const
{
    std::vector<Value> values;

    for (int index : column_indices_)
    {
        values.push_back(
            tuple.get_value(input_schema_, index));
    }

    return Tuple(values, output_schema_);
}

const Schema &Projection::output_schema() const
{
    return output_schema_;
}
