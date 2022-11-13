#pragma once

#include "cache_entry.h"

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	/// <summary>
	/// A doubly-linked list of cache entries
	/// This classes manages data going in and out of the cache
	/// </summary>
	class cache_list
	{
	public:
		cache_list();
		~cache_list();

		/// <summary>
		/// Remove all entries from this list
		/// </summary>
		void clear();

		/// <summary>
		/// How much memory is this list using?
		/// </summary>
		/// <returns></returns>
		size_t size() const { return m_size; }

		// For testing only: What are the beginning and end of this?
		cache_entry* head() const { return m_head; }
		cache_entry* tail() const { return m_tail; }

		/// <summary>
		/// Create and return a cache entry with the given expiration data and date
		/// </summary>
		cache_entry* add(time_t expiration, const byte_array& data);

		/// <summary>
		/// Get the data from a cache entry, touching the entries last access time
		/// </summary>
		const byte_array& get(cache_entry* entry);
		
		/// <summary>
		/// Update the data and expiration for an existing cache entry
		/// </summary>
		void set(cache_entry* entry, time_t expiration, const byte_array& data);

		/// <summary>
		/// Remove a cache entry
		/// </summary>
		void remove(cache_entry* old_entry);
		
		/// <summary>
		/// Remove entries until the list size is below a threshold
		/// </summary>
		void trim(size_t target_size);

	private:
		cache_entry* m_head;
		cache_entry* m_tail;

		size_t m_size;
	};
}
