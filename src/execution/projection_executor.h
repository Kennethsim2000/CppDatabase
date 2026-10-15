#pragma once

#include <string>
#include <vector>
#include "executor.h"
#include "projection.h"

namespace db
{
    class ProjectionExecutor : public Executor
    {
    public:
        ProjectionExecutor(
            Executor &child,
            const Schema &input_schema,
            const std::vector<std::string> &column_names);

        void init() override;
        bool next(Tuple &tuple, RID &rid) override;

        const Schema &output_schema() const;

    private:
        Executor &child_;
        Projection projection_;
    };
}