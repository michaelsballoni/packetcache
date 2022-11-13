#pragma once

#include "cache_list.h"
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
		cache(const size_t max_size);
		~cache();

		const char* handle_packet(const packet& input, packet& output);
		void clear();

	private:
		std::unordered_map<std::vector<uint8_t>, cache_entry*, byte_array_hasher> m_cache;
		cache_list m_cache_list;

		size_t m_max_size;

		uint32_t m_gets, m_puts, m_deletes, m_hits, m_misses;
	};
}
