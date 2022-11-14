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

		bool put(const char* key, const char* value, const int ttl);
		bool get(const char* key, std::string& value);
		bool del(const char* key);

	private:
		bool process_packet(const packet& request, packet& response);

	private:
		SOCKET m_socket;
	};
}