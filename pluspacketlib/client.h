#pragma once

#include "packet.h"
#include <string>

namespace packetcache
{
	class client
	{
	public:
		client();
		~client();

		bool connect(const char* server, const uint16_t port);

		bool put(const std::string& key, const std::string& value, const int ttl);
		bool get(const std::string& key, std::string& value);
		bool del(const std::string& key);

	private:
		bool process_packet();

	private:
		SOCKET m_socket;

		// reuse everything
		packet m_request, m_response;
		std::vector<uint8_t> m_request_buffer, m_response_buffer;
	};
}
