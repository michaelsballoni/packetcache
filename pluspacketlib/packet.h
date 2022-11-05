#pragma once

#include <vector>

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	enum class cache_op
	{
		Get,
		Put,
		Delete,
		Success
	};

	class packet
	{
	public:
		static const size_t max_packet_size = 2 * 1024;

		// Pack a packet
		byte_array pack(const cache_op op, const byte_array& key, const byte_array& value);

		// Parse a packet
		// Does not take ownership of the data, instead storing offsets into the data
		packet(const uint8_t* data, size_t len);

		const cache_op op() { return m_op; }

		const uint8_t* key() const { return m_key; }
		const size_t key_len() const { return m_key_len; }

		const uint8_t* value() const { return m_value; }
		const size_t value_len() const { return m_value_len; }

	private:
		const uint8_t* m_data;
		size_t m_len;

		cache_op m_op;

		uint8_t* m_key;
		size_t m_key_len;

		uint8_t* m_value;
		size_t m_value_len;
	};
}
