#include "tuple.h"

#include <cstring>
#include <stdexcept>
#include <string>

using namespace db;

void Tuple::append_bytes(
    const void *data,
    size_t size)
{
    const char *bytes =
        reinterpret_cast<const char *>(data);

    data_.insert(
        data_.end(),
        bytes,
        bytes + size);
}

Tuple::Tuple(
    const std::vector<Value> &values,
    const Schema &schema)
{
    // Make sure the number of values matches the schema.
    if (values.size() != schema.column_count())
    {
        throw std::invalid_argument(
            "Number of values does not match schema");
    }

    // Serialize each value according to its column type.
    for (size_t i = 0; i < values.size(); i++)
    {
        const Value &value = values[i];
        TypeId type = schema.column(i).type();

        // Make sure the value has the correct type.
        if (value.type() != type)
        {
            throw std::invalid_argument(
                "Tuple value does not match schema");
        }

        switch (type)
        {
        case TypeId::INTEGER:
        {
            int32_t v = value.as_int();
            append_bytes(
                &v,
                sizeof(v));

            break;
        }

        case TypeId::BIGINT:
        {
            int64_t v = value.as_bigint();
            append_bytes(
                &v,
                sizeof(v));

            break;
        }

        case TypeId::BOOLEAN:
        {
            uint8_t v =
                value.as_bool() ? 1 : 0;
            append_bytes(
                &v,
                sizeof(v));

            break;
        }

        case TypeId::VARCHAR:
        {
            const std::string &str =
                value.as_string();

            uint32_t length =
                static_cast<uint32_t>(str.size());

            // Store string length first, followed by actual characters
            append_bytes(
                &length,
                sizeof(length));
            append_bytes(
                str.data(),
                str.size());

            break;
        }
        }
    }
}

Value Tuple::get_value(
    const Schema &schema,
    size_t index) const
{
    if (index >= schema.column_count())
    {
        throw std::out_of_range(
            "Tuple column index out of range");
    }

    size_t offset = 0;

    // Walk through the serialized tuple until we reach the requested column.
    for (size_t i = 0; i <= index; i++)
    {
        TypeId type =
            schema.column(i).type();

        switch (type)
        {
        case TypeId::INTEGER:
        {
            if (offset + sizeof(int32_t) > data_.size())
            {
                throw std::runtime_error(
                    "Corrupted tuple");
            }

            int32_t value;

            std::memcpy(
                &value,
                data_.data() + offset,
                sizeof(value));

            if (i == index)
            {
                return Value::Integer(value);
            }

            offset += sizeof(value);

            break;
        }

        case TypeId::BIGINT:
        {
            if (offset + sizeof(int64_t) > data_.size())
            {
                throw std::runtime_error(
                    "Corrupted tuple");
            }

            int64_t value;

            std::memcpy(
                &value,
                data_.data() + offset,
                sizeof(value));

            if (i == index)
            {
                return Value::BigInt(value);
            }

            offset += sizeof(value);

            break;
        }

        case TypeId::BOOLEAN:
        {
            if (offset + sizeof(uint8_t) > data_.size())
            {
                throw std::runtime_error(
                    "Corrupted tuple");
            }

            uint8_t value;

            std::memcpy(
                &value,
                data_.data() + offset,
                sizeof(value));

            if (i == index)
            {
                return Value::Boolean(value != 0);
            }

            offset += sizeof(value);

            break;
        }

        case TypeId::VARCHAR:
        {
            if (offset + sizeof(uint32_t) > data_.size())
            {
                throw std::runtime_error(
                    "Corrupted tuple");
            }

            uint32_t length;

            std::memcpy(
                &length,
                data_.data() + offset,
                sizeof(length));

            offset += sizeof(length);

            if (offset + length > data_.size())
            {
                throw std::runtime_error(
                    "Corrupted tuple");
            }

            std::string value(
                data_.data() + offset,
                length);

            if (i == index)
            {
                return Value::Varchar(value);
            }

            offset += length;

            break;
        }
        }
    }

    throw std::out_of_range(
        "Tuple column index out of range");
}