#pragma once

#include <string>
#include <vector>

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	enum class cache_op
	{
		Failure,
		Success,

		Clear,

		Get,
		Put,
		Delete,

		GetStats,
		ClearStats,
	};

	// https://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
	const size_t max_packet_size = 508;

	const size_t min_packet_size = 
		sizeof(uint8_t)  + // op 
		sizeof(uint32_t) + // ttl
		sizeof(uint16_t)  + // key len
		sizeof(uint16_t)  + // val len
		sizeof(uint32_t);  // crc

	struct packet
	{
		cache_op op = cache_op::Failure;
		time_t expiration = 0;

		byte_array key;
		byte_array value;

		void reset();
		std::string to_string() const;

		// These return nullptr on success, otherwise an error message
		static const char* pack(const packet& p, uint8_t* output, size_t& output_len);
		static const char* parse(const uint8_t* input, const size_t input_len, packet& p);
	};
}
