#include "projection_executor.h"

using namespace db;

ProjectionExecutor::ProjectionExecutor(
    Executor &child,
    const Schema &input_schema,
    const std::vector<std::string> &column_names)
    : child_(child),
      projection_(input_schema, column_names)
{
}

void ProjectionExecutor::init()
{
    child_.init();
}

bool ProjectionExecutor::next(Tuple &tuple, RID &rid)
{
    Tuple input_tuple;

    if (!child_.next(input_tuple, rid))
    {
        return false;
    }

    tuple = projection_.project(input_tuple);
    return true;
}

const Schema &ProjectionExecutor::output_schema() const
{
    return projection_.output_schema();
}