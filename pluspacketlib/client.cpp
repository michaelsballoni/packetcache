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

	bool client::put(const char* key, const char* value, const int ttl)
	{
		packet request, response;
		request.key = str_to_buffer(key);
		request.value = str_to_buffer(value);
		request.op = cache_op::Put;
		request.expiration = ttl;
		bool result = process_packet(request, response);
		return result && response.op == cache_op::Success;
	}

	bool client::get(const char* key, std::string& value)
	{
		packet request, response;
		request.key = str_to_buffer(key);
		request.op = cache_op::Get;
		bool result = process_packet(request, response);
		if (!result)
			return false;
		if (response.op != cache_op::Success)
			return false;
		value = buffer_to_str(response.value);
		return true;
	}

	bool client::del(const char* key)
	{
		packet request, response;
		request.key = str_to_buffer(key);
		request.op = cache_op::Delete;
		bool result = process_packet(request, response);
		return result && response.op == cache_op::Success;
	}

	bool client::process_packet(const packet& request, packet& response)
	{
		// build out request packet
		std::vector<uint8_t> request_buffer;
		const char* pack_result = packet::pack(request, request_buffer);
		if (pack_result != nullptr)
		{
			printf("packet::pack: %s\n", pack_result);
			return false;
		}

		// send the request
		if (::send(m_socket, (const char*)request_buffer.data(), (int)request_buffer.size(), 0) != 0)
		{
			printf("send: %d\n", (int)::WSAGetLastError());
			return false;
		}

		// read the response
		std::vector<uint8_t> response_buffer;
		response_buffer.resize(max_packet_size);
		int read_amount = ::recv(m_socket, (char*)response_buffer.data(), (int)response_buffer.size(), 0);
		if (read_amount <= 0)
		{	
			printf("recv: %d\n", (int)::WSAGetLastError());
			return false;
		}

		// trim the output buffer
		response_buffer.resize(read_amount);

		// parse the input buffer into a usable packet
		const char* parse_result = packet::parse(response_buffer, response);
		if (parse_result != nullptr)
		{
			printf("packet::parse: %s\n", parse_result);
			return false;
		}

		// all done
		return true;
	}
}
