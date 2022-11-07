#include "pch.h"
#include "cache.h"

using namespace packetcache;

cache::cache()
	: m_gets(0)
	, m_puts(0)
	, m_deletes(0)
	, m_hits(0)
	, m_misses(0)
{
}

cache::~cache()
{
	clear();
}

const char* cache::handle_packet(const packet& input, packet& output)
{
	output.reset();
	output.op = cache_op::Success;

	switch (input.op)
	{
	case cache_op::Get:
	{
		const auto& it = m_cache.find(input.key);
		if (it != m_cache.end())
		{
			// FORNOW - Avoid this copy
			output.value = *it->second; 
		}
		else
			output.op = cache_op::Failure;
		break;
	}

	case cache_op::Put:
	{
		auto it = m_cache.find(input.key);
		if (it != m_cache.end())
		{
			// FORNOW - Avoid this copy, move it in, clear it out for next time
			*it->second = input.value; 
		}
		else
		{
			// FORNOW - Avoid this copy
			m_cache.insert({ input.key, new byte_array(input.value) }); 
		}
		break;
	}

	case cache_op::Delete:
	{
		auto it = m_cache.find(input.key);
		if (it != m_cache.end())
		{
			delete it->second;
			m_cache.erase(it);
		}
		break;
	}

	// FORNOW
	case cache_op::GetStats:
	{
		break;
	}

	// FORNOW
	case cache_op::ClearStats:
	{
		break;
	}

	case cache_op::Clear:
		clear();
		break;

	default:
		return "unrecoginized op";
	}

	// All done
	return nullptr;
}

void cache::clear()
{
	for (auto& it : m_cache)
		delete it.second;
	m_cache.clear();
}
