#include "pch.h"
#include "server.h"
#include "packet.h"

#include <vector>

namespace packetcache
{
	packet_server::packet_server(const uint16_t port, const size_t max_cache_size_bytes)
		: m_port(port)
		, m_cache(max_cache_size_bytes)
		, m_socket(INVALID_SOCKET)
		, m_keepRunning(false)
	{
	}

	packet_server::~packet_server()
	{
		stop();
	}

	bool packet_server::start()
	{
		stop();

		// create the UDP socket
		m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socket == INVALID_SOCKET)
		{
			int last_error = ::WSAGetLastError();
			printf("socket: %d\n", last_error);
			return false;
		}

		// bind the socket to our port
		sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET; 
		servaddr.sin_addr.s_addr = INADDR_ANY;
		servaddr.sin_port = htons(m_port);
		if (::bind(m_socket, (const sockaddr*)&servaddr, sizeof(servaddr)) != 0)
		{
			int last_error = ::WSAGetLastError();
			printf("bind: %d\n", last_error);
			return false;
		}

		// all good
		m_keepRunning = true;
		m_thread = std::make_shared<std::thread>(&packet_server::process_requests, this);
		return true;
	}

	void packet_server::stop()
	{
		m_keepRunning = false;

		if (m_socket != INVALID_SOCKET)
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		if (m_thread != nullptr)
		{
			m_thread->join();
			m_thread.reset();
		}
	}

	void packet_server::process_requests()
	{
		// lift everything out of the loop
		std::vector<uint8_t> input_buffer, output_buffer;

		packet input_packet, output_packet;

		sockaddr_in clientaddr;
		sockaddr* clientaddr_addr = (sockaddr*)&clientaddr;
		memset(clientaddr_addr, 0, sizeof(clientaddr));

		int len = -1;
		
		while (m_keepRunning)
		{
			// reset our input buffer
			input_buffer.resize(max_packet_size);

			// read the request
			len = sizeof(clientaddr);
			int read_amount =
				::recvfrom(m_socket, (char*)input_buffer.data(), (int)input_buffer.size(), 0, clientaddr_addr, &len);
			if (read_amount <= 0)
			{
				int last_error = ::WSAGetLastError();
				if (m_keepRunning)
					printf("recvfrom: %d\n", last_error);
				continue;
			}

			// trim the input buffer
			input_buffer.resize(read_amount);

			// parse the input buffer into a usable packet
			bool success = true;
			if (success)
			{
				const char* parse_result = packet::parse(input_buffer, input_packet);
				if (parse_result != nullptr)
				{
					printf("packet::parse: %s\n", parse_result);
					output_packet.op = cache_op::Failure;
					success = false;
				}
			}

			// process the packet
			if (success)
			{
				const char* handle_result = m_cache.handle_packet(input_packet, output_packet);
				if (handle_result != nullptr)
				{
					printf("handle_packet: %s\n", handle_result);
					output_packet.op = cache_op::Failure;
					success = false;
				}
			}

			// pack the response into the output buffer
			const char* pack_result = packet::pack(output_packet, output_buffer);
			if (pack_result != nullptr)
			{
				printf("packet::pack: %s\n", pack_result);
				output_packet.op = cache_op::Failure;
			}

			// send the response
			if (::sendto(m_socket, (const char*)output_buffer.data(), (int)output_buffer.size(), 0, (const sockaddr*)&clientaddr, len) < 0)
			{
				int last_error = ::WSAGetLastError();
				if (m_keepRunning)
					printf("sendto: %d\n", last_error);
				continue;
			}
		}
	}
}
