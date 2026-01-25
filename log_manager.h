// ================================
// File: include/db/log_manager.h
// Write-Ahead Logging (WAL)
// ================================
#pragma once

#include "types.h"

namespace db
{

    class LogManager
    {
    public:
        // Append a log record
        void append(TxnId txn, const std::string &record);

        // Flush logs to disk
        void flush();

    private:
        // TODO: log buffer, LSN tracking
    };

} // namespace db