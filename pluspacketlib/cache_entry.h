#pragma once

#include <vector>
#include <time.h>

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	struct cache_entry
	{
		cache_entry(time_t _expiration, const byte_array& _data, cache_entry*& head)
			: expiration(_expiration)
			, last_access(::time(nullptr))
			, prev(nullptr)
			, next(nullptr)
			, data(_data)
		{
			if (head != nullptr)
			{
				head->prev = this;

				next = head;
				prev = nullptr;
			}
			head = this;
		}

		time_t expiration;
		time_t last_access;

		cache_entry* prev;
		cache_entry* next;

		byte_array data;

		size_t size() const
		{
			return
				2 * sizeof(time_t) +
				3 * sizeof(cache_entry*) + // self, prev, next
				data.size() + sizeof(size_t);
		}
	};
}
