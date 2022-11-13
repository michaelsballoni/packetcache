#include "pch.h"
#include "cache.h"

using namespace packetcache;

cache::cache(const size_t max_size)
	: m_gets(0)
	, m_puts(0)
	, m_deletes(0)
	, m_hits(0)
	, m_misses(0)
	, m_max_size(max_size)
{
}

cache::~cache()
{
	clear();
}

const char* cache::handle_packet(const packet& input, packet& output)
{
	output.reset();

	switch (input.op)
	{
	case cache_op::Get:
	{
		++m_gets;

		const auto& it = m_cache.find(input.key);
		if (it != m_cache.end())
		{
			if (it->second->expiration < ::time(nullptr))
			{
				auto it2 = m_cache.find(input.key);
				
				m_cache_list.remove(it2->second);
				m_cache.erase(it2);

				++m_misses;
				output.op = cache_op::Failure;
			}
			else
			{
				++m_hits;
				output.op = cache_op::Success;
				output.value = m_cache_list.get(it->second);
			}
		}
		else
		{
			++m_misses;
			output.op = cache_op::Failure;
		}
		break;
	}

	case cache_op::Put:
	{
		++m_puts;
		output.op = cache_op::Success;

		auto it = m_cache.find(input.key);
		if (it != m_cache.end())
			m_cache_list.set(it->second, input.expiration, input.value);
		else
			m_cache.insert({ input.key, m_cache_list.add(input.expiration, input.value)});
		
		m_cache_list.trim(m_max_size);
		break;
	}

	case cache_op::Delete:
	{
		++m_deletes;
		output.op = cache_op::Success;

		auto it = m_cache.find(input.key);
		if (it != m_cache.end())
		{
			m_cache_list.remove(it->second);
			m_cache.erase(it);
		}
		break;
	}

	case cache_op::GetStats:
	{
		output.op = cache_op::Success;

		char stats_str[1024];
		::sprintf_s
		(
			stats_str,
			sizeof(stats_str),
			"gets: %u - puts: %u - deletes: %u - hits: %u - misses: %u",
			m_gets,
			m_puts,
			m_deletes,
			m_hits,
			m_misses
		);
		output.value = str_to_buffer(stats_str);
		break;
	}

	case cache_op::ClearStats:
	{
		output.op = cache_op::Success;

		m_gets = 0;
		m_puts = 0;
		m_deletes = 0;
		m_hits = 0;
		m_misses = 0;

		break;
	}

	case cache_op::Clear:
	{
		output.op = cache_op::Success;
		clear();
		break;
	}

	default:
		return "unrecoginized op";
	}

	return nullptr; // success!
}

void cache::clear()
{
	m_cache.clear();
	m_cache_list.clear();
}
