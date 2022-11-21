#include "pch.h"
#include "packet.h"
#include "utils.h"

#include <stdexcept>
#include <sstream>

namespace packetcache
{
	void packet::reset()
	{
		op = cache_op::Failure;
		expiration = 0;

		key.clear();
		value.clear();
	}

	std::string packet::to_string() const
	{
		std::stringstream str;
		
		std::string key_str;
		key_str.append((const char*)key.data(), key.size());

		std::string val_str;
		val_str.append((const char*)value.data(), value.size());

		str << "op: " << (int)op << " - ttl: " << expiration << " - key: " << key_str << " - value: " << val_str;
		return str.str();
	}

	const char* packet::pack(const packet& p, uint8_t* output, size_t& output_len)
	{
		// Reset the output
		output_len = 0;

		// Make sure it will all fit
		if
		(
			sizeof(uint8_t) +  // op
			sizeof(uint32_t) + // ttl
			sizeof(u_short) + p.key.size() +
			sizeof(u_short) + p.value.size() +
			sizeof(uint32_t)   // crc
			> max_packet_size
		)
		{
			return "too much data";
		}

		// Add the op
		output[output_len++] = uint8_t(p.op);

		// Add the TTL
		uint32_t ttl_net = htonl(uint32_t(p.expiration));
		uint8_t* ttl_data = reinterpret_cast<uint8_t*>(&ttl_net);
		output[output_len++] = ttl_data[0];
		output[output_len++] = ttl_data[1];
		output[output_len++] = ttl_data[2];
		output[output_len++] = ttl_data[3];

		// Add the key len and data
		u_short key_len_net = htons(u_short(p.key.size()));
		uint8_t* key_len_data = reinterpret_cast<uint8_t*>(&key_len_net);
		output[output_len++] = key_len_data[0];
		output[output_len++] = key_len_data[1];
		memcpy(output + output_len, p.key.data(), p.key.size());
		output_len += p.key.size();

		// Add the value len and data
		u_short value_len_net = htons(u_short(p.value.size()));
		uint8_t* value_len_data = reinterpret_cast<uint8_t*>(&value_len_net);
		output[output_len++] = value_len_data[0];
		output[output_len++] = value_len_data[1];
		memcpy(output + output_len, p.value.data(), p.value.size());
		output_len += p.value.size();

		// CRC what we've got so far, and add to the output
		const uint32_t crc_value = crc_bytes(output, output_len);
		// DEBUG
		//printf("pack crc_value = %u\n", crc_value);
		const uint32_t crc_net = htonl(crc_value);
		const uint8_t* crc_data = reinterpret_cast<const uint8_t*>(&crc_net);
		output[output_len++] = crc_data[0];
		output[output_len++] = crc_data[1];
		output[output_len++] = crc_data[2];
		output[output_len++] = crc_data[3];

		// All done
		return nullptr;
	}

	// Parse a packet
	const char* packet::parse(const uint8_t* input, const size_t input_len, packet& p)
	{
		// Reset the the output
		p.reset();

		// Validate size
		if (input_len < min_packet_size)
			return "too small";
		else if (input_len > max_packet_size)
			return "too big";

		// Validate the CRC
		const uint32_t crc_computed = crc_bytes(input, input_len - sizeof(uint32_t));
		const uint32_t crc_given = ntohl(*reinterpret_cast<const u_long*>(input + input_len - sizeof(uint32_t)));
		// DEBUG
		//printf("pack crc_computed = %u\n", crc_computed);
		//printf("pack crc_given =    %u\n", crc_given);
		if (crc_given != crc_computed)
			return "checksum mismatch";

		// Extract op
		size_t idx = 0;
		p.op = cache_op(input[idx++]);

		// Extract ttl
		const uint32_t ttl = ntohl(*reinterpret_cast<const uint32_t*>(input + idx));
		idx += sizeof(ttl);
		p.expiration = ::time(nullptr) + ttl;

		// Extract key
		const u_short key_len = ntohs(*reinterpret_cast<const u_short*>(input + idx));
		idx += sizeof(key_len);
		if (key_len > 0)
		{
			if (idx + key_len >= input_len)
				return "invalid key len";

			p.key.resize(key_len);
			memcpy(p.key.data(), input + idx, key_len);

			idx += key_len;
		}

		// Extract value
		const u_short value_len = ntohs(*reinterpret_cast<const u_short*>(input + idx));
		idx += sizeof(value_len);
		if (value_len > 0)
		{
			if (idx + value_len >= input_len)
				return "invalid value len";

			p.value.resize(value_len);
			memcpy(p.value.data(), input + idx, value_len);

			idx += value_len;
		}

		// Validate packet size
		if (idx != input_len - 4)
			return "invalid packet format";

		return nullptr;
	}
}
