#pragma once

#include "cache.h"

#include <memory>
#include <thread>

namespace packetcache
{
	class packet_server
	{
	public:
		packet_server(const uint16_t port, const size_t max_cache_size_bytes);
		~packet_server();

		bool start();
		void stop();

	private:
		void process_requests();

	private:
		const uint16_t m_port;

		SOCKET m_socket;
		
		bool m_keepRunning;

		cache m_cache;

		std::shared_ptr<std::thread> m_thread;
	};
}
