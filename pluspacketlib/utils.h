#pragma once

#include <string>
#include <vector>

namespace packetcache
{
    uint32_t hash_bytes(const uint8_t* data, const size_t len);
    uint32_t crc_bytes(const uint8_t* data, const size_t len);

    std::vector<uint8_t> str_to_buffer(const char* str);
    std::string buffer_to_str(const std::vector<uint8_t>& buffer);

    struct byte_array_hasher
    {
        std::size_t operator()(const std::vector<uint8_t>& v) const noexcept
        {
            return packetcache::hash_bytes(v.data(), v.size());
        }
    };
}

// Support unordered_map with a std::vector<uint8_t> key
// Not really different from a std::string key
inline bool operator==(const std::vector<uint8_t>& vec1, const std::vector<uint8_t>& vec2)
{
    if (vec2.size() != vec1.size())
        return false;
    for (size_t idx = 0; idx < vec1.size(); ++idx)
    {
        if (vec2[idx] != vec1[idx])
            return false;
    }
    return true;
}
