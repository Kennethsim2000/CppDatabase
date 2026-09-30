#pragma once

#include "types.h"
#include "comparison.h"

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

        bool compare(
            ComparisonType comparison,
            const Value &other) const
        {
            if (type_ != other.type_)
            {
                throw std::invalid_argument(
                    "Cannot compare Values of different types");
            }

            switch (type_)
            {
            case TypeId::INTEGER:
            {
                int32_t left = as_int();
                int32_t right = other.as_int();

                switch (comparison)
                {
                case ComparisonType::EQUAL:
                    return left == right;

                case ComparisonType::NOT_EQUAL:
                    return left != right;

                case ComparisonType::LESS_THAN:
                    return left < right;

                case ComparisonType::LESS_THAN_OR_EQUAL:
                    return left <= right;

                case ComparisonType::GREATER_THAN:
                    return left > right;

                case ComparisonType::GREATER_THAN_OR_EQUAL:
                    return left >= right;
                }

                break;
            }

            case TypeId::BIGINT:
            {
                int64_t left = as_bigint();
                int64_t right = other.as_bigint();

                switch (comparison)
                {
                case ComparisonType::EQUAL:
                    return left == right;

                case ComparisonType::NOT_EQUAL:
                    return left != right;

                case ComparisonType::LESS_THAN:
                    return left < right;

                case ComparisonType::LESS_THAN_OR_EQUAL:
                    return left <= right;

                case ComparisonType::GREATER_THAN:
                    return left > right;

                case ComparisonType::GREATER_THAN_OR_EQUAL:
                    return left >= right;
                }

                break;
            }

            case TypeId::BOOLEAN:
            {
                bool left = as_bool();
                bool right = other.as_bool();

                switch (comparison)
                {
                case ComparisonType::EQUAL:
                    return left == right;

                case ComparisonType::NOT_EQUAL:
                    return left != right;

                case ComparisonType::LESS_THAN:
                case ComparisonType::LESS_THAN_OR_EQUAL:
                case ComparisonType::GREATER_THAN:
                case ComparisonType::GREATER_THAN_OR_EQUAL:
                    throw std::invalid_argument(
                        "Ordering comparison is not supported for BOOLEAN");
                }

                break;
            }

            case TypeId::VARCHAR:
            {
                std::string left = as_string();
                std::string right = other.as_string();

                switch (comparison)
                {
                case ComparisonType::EQUAL:
                    return left == right;

                case ComparisonType::NOT_EQUAL:
                    return left != right;

                case ComparisonType::LESS_THAN:
                    return left < right;

                case ComparisonType::LESS_THAN_OR_EQUAL:
                    return left <= right;

                case ComparisonType::GREATER_THAN:
                    return left > right;

                case ComparisonType::GREATER_THAN_OR_EQUAL:
                    return left >= right;
                }

                break;
            }
            }

            throw std::invalid_argument("Invalid comparison");
        }

    private:
        TypeId type_;

        int32_t int_value_ = 0;
        int64_t bigint_value_ = 0;
        bool bool_value_ = false;
        std::string string_value_;
    };

}