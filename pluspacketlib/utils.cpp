#include "pch.h"
#include "utils.h"

#include <functional>
#include <vector>

#include <zlib.h>

#include "MurmurHash2.h"

uint32_t packetcache::hash_byte_array(const std::vector<uint8_t>& v)
{
    return MurmurHash2(v.data(), static_cast<int>(v.size()), 0);
}

uint32_t packetcache::crc_byte_array(const std::vector<uint8_t>& v)
{
    uint32_t crc = crc32(0L, Z_NULL, 0);
    return crc32(crc, v.data(), static_cast<uint32_t>(v.size()));
}
