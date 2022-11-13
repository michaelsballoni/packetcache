#include "pch.h"
#include "cache_list.h"

namespace packetcache
{
	cache_list::cache_list()
		: m_head(nullptr)
		, m_tail(nullptr)
		, m_size(0)
	{
	}

	cache_list::~cache_list()
	{
		clear();
	}

	void cache_list::clear()
	{
		// delete all entries
		cache_entry* c = m_head;
		while (c != nullptr)
		{
			cache_entry* next = c->next;
			delete c;
			c = next;
		}

		// reset
		m_head = nullptr;
		m_tail = nullptr;
		
		m_size = 0;
	}

	cache_entry* cache_list::add(time_t expiration, const byte_array& data)
	{
		// create new entry, making the new entry head of the list
		cache_entry* new_entry = new cache_entry(expiration, data, m_head);

		// set the tail if none
		if (m_tail == nullptr)
			m_tail = new_entry;

		// there's more in the list
		m_size += new_entry->size();

		return new_entry;
	}

	const byte_array& cache_list::get(cache_entry* entry)
	{
		entry->last_access = ::time(nullptr);

		//
		// move the entry to the front of the list
		//

		// bail if already at the head
		if (m_head == entry)
			return entry->data;

		// adjust neighbor pointers
		auto prev_neighbor = entry->prev;
		auto next_neighbor = entry->next;

		if (prev_neighbor != nullptr)
			prev_neighbor->next = next_neighbor;

		if (next_neighbor != nullptr)
			next_neighbor->prev = prev_neighbor;

		// make the new entry head of the list
		if (m_head != nullptr)
		{
			m_head->prev = entry;

			entry->next = m_head;
			entry->prev = nullptr;
		}
		m_head = entry;

		// set the tail
		if (next_neighbor != nullptr && next_neighbor->next == nullptr)
			m_tail = next_neighbor;
		else if (prev_neighbor != nullptr && prev_neighbor->next == nullptr)
			m_tail = prev_neighbor;

		return entry->data;
	}

	void cache_list::set(cache_entry* entry, time_t expiration, const byte_array& data)
	{
		entry->last_access = ::time(nullptr);

		size_t old_size = entry->data.size();

		entry->expiration = expiration;
		entry->data = data;

		// adjust the new size carefully
		size_t new_size = data.size();
		int64_t size_change = int64_t(new_size) - old_size;
		if (-size_change > int64_t(m_size))
			m_size = 0;
		else
			m_size += size_change;
	}

	void cache_list::remove(cache_entry* old_entry)
	{
		// update head and tail
		if (m_head == old_entry)
			m_head = old_entry->next;

		if (m_tail == old_entry)
			m_tail = old_entry->prev;

		// update neighbors
		if (old_entry->prev != nullptr)
			old_entry->prev->next = old_entry->next;

		if (old_entry->next != nullptr)
			old_entry->next->prev = old_entry->prev;

		// adjust our size
		int64_t size_change = old_entry->size();
		if (size_change > int64_t(m_size))
			m_size = 0;
		else
			m_size -= size_change;

		// clean it up
		delete old_entry;
	}

	void cache_list::trim(size_t target_size)
	{
		while (m_size > target_size && m_tail != nullptr)
			remove(m_tail);
	}
}
