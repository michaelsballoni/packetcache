#include "pch.h"
#include "client.h"
#include "utils.h"

namespace packetcache
{
	client::client()
		: m_socket(INVALID_SOCKET)
	{
	}

	client::~client()
	{
		::closesocket(m_socket);
	}

	bool client::connect(const char* server, const uint16_t port)
	{
		if (m_socket != INVALID_SOCKET)
			::closesocket(m_socket);

		sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_addr.s_addr = inet_addr(server);
		servaddr.sin_port = htons(port);
		servaddr.sin_family = AF_INET;
		m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
		if (m_socket == INVALID_SOCKET)
		{
			printf("socket: %d\n", (int)::WSAGetLastError());
			return false;
		}

		if (::connect(m_socket, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
		{
			printf("connect: %d\n", (int)::WSAGetLastError());
			return false;
		}

		return true;
	}

	bool client::put(const std::string& key, const std::string& value, const int ttl)
	{
		str_to_buffer(key, m_request.key);
		str_to_buffer(value, m_request.value);

		m_request.op = cache_op::Put;
		m_request.expiration = ttl;

		bool result = process_packet();
		return result && m_response.op == cache_op::Success;
	}

	bool client::get(const std::string& key, std::string& value)
	{
		str_to_buffer(key, m_request.key);

		m_request.op = cache_op::Get;

		bool result = process_packet();
		if (!result)
			return false;
		if (m_response.op != cache_op::Success)
			return false;

		value = buffer_to_str(m_response.value);
		return true;
	}

	bool client::del(const std::string& key)
	{
		str_to_buffer(key, m_request.key);
		m_request.op = cache_op::Delete;
		bool result = process_packet();
		return result && m_response.op == cache_op::Success;
	}

	bool client::process_packet()
	{
		// build out request packet
		const char* pack_result = packet::pack(m_request, m_request_buffer);
		if (pack_result != nullptr)
		{
			printf("packet::pack: %s\n", pack_result);
			return false;
		}

		// send the request
		if (::send(m_socket, (const char*)m_request_buffer.data(), (int)m_request_buffer.size(), 0) != 0)
		{
			printf("send: %d\n", (int)::WSAGetLastError());
			return false;
		}

		// read the response
		m_response_buffer.resize(max_packet_size);
		int read_amount = ::recv(m_socket, (char*)m_response_buffer.data(), (int)m_response_buffer.size(), 0);
		if (read_amount <= 0)
		{	
			printf("recv: %d\n", (int)::WSAGetLastError());
			return false;
		}

		// trim the output buffer
		m_response_buffer.resize(read_amount);

		// parse the input buffer into a usable packet
		const char* parse_result = packet::parse(m_response_buffer, m_response);
		if (parse_result != nullptr)
		{
			printf("packet::parse: %s\n", parse_result);
			return false;
		}

		// all done
		return true;
	}
}
