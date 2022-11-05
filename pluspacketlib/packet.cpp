#include "pch.h"
#include "packet.h"
#include "utils.h"

#include <stdexcept>

namespace packetcache
{
	byte_array packet::pack(const cache_op op, const byte_array& key, const byte_array& value)
	{
		std::vector<uint8_t> output;

		// Make sure it will all fit
		if (1 + 2 + key.size() + 2 + value.size() + 4 > max_packet_size)
			return output; // kind of null, no valid packet is empty

		// One alloc to rule them all
		output.reserve(max_packet_size);

		// Add the op
		output.push_back(static_cast<uint8_t>(op));

		// Add the key len and data
		u_short key_len_net = htons(static_cast<u_short>(key.size()));
		uint8_t* key_len_data = reinterpret_cast<uint8_t * >(&key_len_net);
		output.insert(output.end(), { key_len_data[0], key_len_data[1] });
		output.insert(output.end(), key.begin(), key.end());

		// Add the value len and data
		u_short value_len_net = htons(static_cast<u_short>(value.size()));
		uint8_t* value_len_data = reinterpret_cast<uint8_t*>(&value_len_net);
		output.insert(output.end(), { value_len_data[0], value_len_data[1] });
		output.insert(output.end(), value.begin(), value.end());

		// CRC what we've got so far, and add to the output
		u_long crc_net = htonl(crc_bytes(output.data(), output.size()));
		uint8_t* crc_data = reinterpret_cast<uint8_t*>(&crc_net);
		output.insert(output.end(), { crc_data[0], crc_data[1], crc_data[2], crc_data[3] });

		// All done
		return output;
	}

	// Parse a packet
	packet::packet(const uint8_t* data, size_t len)
		: m_data(data)
		, m_len(len)
		, m_op(cache_op(0))
		, m_key(nullptr)
		, m_key_len(0)
		, m_value(nullptr)
		, m_value_len(0)
	{
		// FORNOW
		// validate crc32
		// extract op
		// extract key len and pointer
		// extract value len and pointer
	}
}
