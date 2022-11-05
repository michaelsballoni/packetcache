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
