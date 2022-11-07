#include "pch.h"
#include "packet.h"
#include "utils.h"

#include <stdexcept>

namespace packetcache
{
	const char* packet::pack(const packet& p, byte_array& output)
	{
		// Reset the output
		output.clear();
		output.reserve(max_packet_size);

		// Make sure it will all fit
		if
		(
			sizeof(uint8_t) + // op
			sizeof(u_short) + p.key.size() +
			sizeof(u_short) + p.value.size() +
			sizeof(uint32_t) // crc
			> max_packet_size
		)
		{
			return "too much data";
		}

		// Add the op
		output.push_back(uint8_t(p.op));

		// Add the key len and data
		u_short key_len_net = htons(u_short(p.key.size()));
		uint8_t* key_len_data = reinterpret_cast<uint8_t*>(&key_len_net);
		output.insert(output.end(), { key_len_data[0], key_len_data[1] });
		output.insert(output.end(), p.key.begin(), p.key.end());

		// Add the value len and data
		u_short value_len_net = htons(u_short(p.value.size()));
		uint8_t* value_len_data = reinterpret_cast<uint8_t*>(&value_len_net);
		output.insert(output.end(), { value_len_data[0], value_len_data[1] });
		output.insert(output.end(), p.value.begin(), p.value.end());

		// CRC what we've got so far, and add to the output
		uint32_t crc_net = htonl(crc_bytes(output.data(), output.size()));
		uint8_t* crc_data = reinterpret_cast<uint8_t*>(&crc_net);
		output.insert(output.end(), { crc_data[0], crc_data[1], crc_data[2], crc_data[3] });

		// All done
		return nullptr;
	}

	// Parse a packet
	const char* packet::parse(const byte_array& input, packet& p)
	{
		// Reset the the output
		p.reset();

		// Validate the CRC
		if (input.size() < min_packet_size)
			return "too small";
		const uint32_t crc_computed = crc_bytes(input.data(), input.size() - sizeof(uint32_t));
		const uint32_t crc_given = ntohl(*reinterpret_cast<const u_long*>(input.data() + input.size() - sizeof(uint32_t)));
		if (crc_given != crc_computed)
			return "checksum mismatch";

		// Extract op
		size_t idx = 0;
		p.op = cache_op(input[idx++]);

		// Extract key
		const u_short key_len = ntohs(*reinterpret_cast<const u_short*>(input.data() + idx));
		idx += sizeof(u_short);
		if (key_len > 0)
		{
			if (idx + key_len >= input.size())
				return "invalid key len";

			p.key.resize(key_len);
			memcpy(p.key.data(), input.data() + idx, key_len);
			idx += key_len;
		}

		// Extract value
		const u_short value_len = ntohs(*reinterpret_cast<const u_short*>(input.data() + idx));
		idx += sizeof(u_short);
		if (value_len > 0)
		{
			if (idx + value_len >= input.size())
				return "invalid value len";

			p.value.resize(value_len);
			memcpy(p.value.data(), input.data() + idx, value_len);
			idx += value_len;
		}

		return nullptr;
	}
}
