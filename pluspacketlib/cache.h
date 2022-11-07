#pragma once

#include "packet.h"
#include "utils.h"

#include <atomic>
#include <unordered_map>

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	class cache
	{
	public:
		cache();
		~cache();

		const char* handle_packet(const packet& input, packet& output);
		void clear();

	private:
		std::unordered_map<std::vector<uint8_t>, byte_array*, byte_array_hasher> m_cache;
		std::atomic<size_t> m_gets, m_puts, m_deletes, m_hits, m_misses;
	};
}
