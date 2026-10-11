#pragma once

#include "tuple.h"
#include "rid.h"

namespace db
{
    class Executor
    {
    public:
        virtual ~Executor() = default;

        // Prepare the executor for execution.
        virtual void init() = 0;

        // Retrieve the next tuple.
        // Returns false when there are no more tuples.
        virtual bool next(Tuple &tuple, RID &rid) = 0;
    };
}