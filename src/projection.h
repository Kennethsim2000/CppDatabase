#pragma once

#include <vector>

#include "schema.h"
#include "tuple.h"

namespace db
{
    class Projection
    {
    public:
        Projection(
            const Schema &input_schema,
            const std::vector<std::string> &column_names);

        Tuple project(const Tuple &tuple) const;

        const Schema &output_schema() const;

    private:
        const Schema &input_schema_;
        Schema output_schema_; // the tuple stores only serialized values, the output schema is used to tell us how to intepret the columns
        std::vector<int> column_indices_;
    };
}