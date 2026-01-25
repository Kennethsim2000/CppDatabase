// ================================
// File: include/db/serialization.h
// Handles encoding/decoding objects to raw bytes
// ================================
#pragma once

#include <vector>
#include <cstdint>

namespace db
{

    class Serializer
    {
    public:
        // Serialize an object into bytes
        template <typename T>
        static std::vector<uint8_t> serialize(const T &obj);

        // Deserialize bytes into an object
        template <typename T>
        static T deserialize(const std::vector<uint8_t> &data);
    };

} // namespace db