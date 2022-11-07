#pragma once

#include <vector>

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	enum class cache_op
	{
		Success,
		Failure,

		Clear

		Get,
		Put,
		Delete,

		GetStats,
		ClearStats,

	};

	// https://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
	const size_t max_packet_size = 508;

	const size_t min_packet_size = 
		sizeof(uint8_t) + // op 
		sizeof(u_short) + // key len
		sizeof(u_short) + // val len
		sizeof(uint32_t); // crc

	struct packet
	{
		packet() 
			: op(cache_op::Success)
		{
		}

		cache_op op;
		byte_array key;
		byte_array value;

		void reset()
		{
			op = cache_op(0);
			key.clear();
			value.clear();
		}

		static const char* pack(const packet& p, byte_array& output);
		static const char* parse(const byte_array& input, packet& p);
	};
}
