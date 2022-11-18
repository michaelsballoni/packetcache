#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include "server.h"
#pragma comment(lib, "pluspacketlib.lib")

#include <iostream>
#include <limits>
#include <string>

#undef max

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "Usage: packetserver <port> <max size GB>" << std::endl;
		return 0;
	}

	WSADATA data;
	if (::WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "WSAStartup failed: " << ::WSAGetLastError() << std::endl;
		return 1;
	}

	uint16_t port = 0;
	try
	{
		int port_32 = std::stoi(argv[1]);
		if (port_32 <= 0 || port_32 > USHRT_MAX)
			throw std::exception("Out of range");
		else
			port = uint16_t(port_32);
	}
	catch (const std::exception& exp)
	{
		std::cout << "Invalid port number: " << argv[1] << " (" << exp.what() << ")" << std::endl;
		return 1;
	}

	uint64_t max_cache_bytes = 0;
	try
	{
		double max_val = std::stof(argv[2]);
		if (max_val <= 0)
			throw std::exception("Out of range");
		else
			max_cache_bytes = uint64_t(max_val * 1024 * 1024 * 1024 + 0.5);

		if (max_cache_bytes > std::numeric_limits<size_t>::max())
			throw std::exception("Out of range");
	}
	catch (const std::exception& exp)
	{
		std::cout << "Invalid max cache size: " << argv[2] << " (" << exp.what() << ")" << std::endl;
		return 1;
	}

	std::cout << "Starting server... " << std::flush;
	packetcache::packet_server server(port, max_cache_bytes);
	if (!server.start())
		return 1;

	std::cout 
		<< "Server started!" << std::endl 
		<< std::endl 
		<< "Hit[Enter] to stop the server and quit: " << std::flush;

	std::string str;
	std::getline(std::cin, str);
	
	std::cout << std::endl << "Stopping server... " << std::flush;
	server.stop();
	std::cout
		<< "Server stopped!" << std::endl
		<< std::endl
		<< "All done." << std::endl;

	return 0;
}
