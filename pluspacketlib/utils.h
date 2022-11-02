#include <vector>

namespace packetcache
{
    uint32_t hash_byte_array(const std::vector<uint8_t>& v);
    uint32_t crc_byte_array(const std::vector<uint8_t>& v);
}

// Support unordered_map with a std::vector<uint8_t> key
template<>
struct std::hash<std::vector<uint8_t>>
{
    std::size_t operator()(const std::vector<uint8_t>& v) const noexcept
    {
        return packetcache::hash_byte_array(v);
    }
};
