#include <vector>

namespace packetcache
{
    uint32_t hash_bytes(const uint8_t* data, const size_t len);
    uint32_t crc_bytes(const uint8_t* data, const size_t len);
}

// Support unordered_map with a std::vector<uint8_t> key
// Not really different from a std::string key
template<>
struct std::hash<std::vector<uint8_t>>
{
    std::size_t operator()(const std::vector<uint8_t>& v) const noexcept
    {
        return packetcache::hash_bytes(v.data(), v.size());
    }
};
