#pragma once

#include "types.h"

namespace db
{
    class Value
    {
    public:
        static Value Integer(int32_t value) // factory methods to create a value
        {
            Value v;
            v.type_ = TypeId::INTEGER;
            v.int_value_ = value;
            return v;
        }

        static Value BigInt(int64_t value)
        {
            Value v;
            v.type_ = TypeId::BIGINT;
            v.bigint_value_ = value;
            return v;
        }

        static Value Boolean(bool value)
        {
            Value v;
            v.type_ = TypeId::BOOLEAN;
            v.bool_value_ = value;
            return v;
        }

        static Value Varchar(const std::string &value)
        {
            Value v;
            v.type_ = TypeId::VARCHAR;
            v.string_value_ = value;
            return v;
        }

        TypeId type() const
        {
            return type_;
        }

        int32_t as_int() const
        {
            return int_value_;
        }

        int64_t as_bigint() const
        {
            return bigint_value_;
        }

        bool as_bool() const
        {
            return bool_value_;
        }

        const std::string &as_string() const
        {
            return string_value_;
        }

    private:
        TypeId type_;

        int32_t int_value_ = 0;
        int64_t bigint_value_ = 0;
        bool bool_value_ = false;
        std::string string_value_;
    };

}