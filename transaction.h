// ================================
// File: include/db/transaction.h
// Transaction metadata and state
// ================================
#pragma once

#include "types.h"

namespace db
{

    enum class TxnState
    {
        ACTIVE,
        COMMITTED,
        ABORTED
    };

    class Transaction
    {
    public:
        explicit Transaction(TxnId id);

        TxnId id() const;
        TxnState state() const;

        void commit();
        void abort();

    private:
        TxnId id_;
        TxnState state_;

        // TODO: write set, undo log
    };

} // namespace db