#include "pch.h"
#include "utils.h"

#include <functional>
#include <vector>

#include <zlib.h>

#include "MurmurHash2.h"

uint32_t packetcache::hash_bytes(const uint8_t* data, const size_t len)
{
    return MurmurHash2(data, static_cast<int>(len), 0);
}

uint32_t packetcache::crc_bytes(const uint8_t* data, const size_t len)
{
    uint32_t crc = crc32(0L, Z_NULL, 0);
    return crc32(crc, data, static_cast<uint32_t>(len));
}

std::vector<uint8_t> packetcache::str_to_buffer(const std::string& str)
{
    std::vector<uint8_t> vec;
    str_to_buffer(str, vec);
    return vec;
}

void packetcache::str_to_buffer(const std::string& str, std::vector<uint8_t>& output)
{
    output.resize(str.size());
    memcpy(output.data(), str.c_str(), str.size());
}

std::string packetcache::buffer_to_str(const std::vector<uint8_t>& buffer)
{
    std::string str;
    buffer_to_str(buffer, str);
    return str;
}

void packetcache::buffer_to_str(const std::vector<uint8_t>& buffer, std::string& output)
{
    output.resize(buffer.size());
    memcpy(const_cast<char*>(output.c_str()), buffer.data(), buffer.size());
}
