#include <iostream>
#include <string>
#include <cstdint> // Pre-defined typedefs
#include <stdexcept>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#define recvbuflen 1024

class TCP_server
{
private:
	WSADATA wsa_data;
	SOCKET l_socket = INVALID_SOCKET; // listen socket
	SOCKET c_socket = INVALID_SOCKET; // client socket, for accepting connections from the client.
	addrinfo* result = NULL, * ptr = NULL, hints;

	int i_result, i_send_result = 0;

	bool ready[3] = { false, false, false }; // Initialized, bound to port, listening

	char recvbuff[recvbuflen];

	std::uint64_t c_port = 0; // current port
public:
	TCP_server() { init(); }
	TCP_server(std::uint64_t port) { init(port); }

	void init()
	{
		if (!ready[0])
		{
			i_result = 0;
			i_send_result = 0;
			c_port = 0;
			l_socket = INVALID_SOCKET;
			c_socket = INVALID_SOCKET;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET; // is used to specify the IPv4 address family.
			hints.ai_socktype = SOCK_STREAM; // is used to specify a stream socket.
			hints.ai_protocol = IPPROTO_TCP; // is used to specify the TCP protocol.
			hints.ai_flags = AI_PASSIVE; // flag indicates the caller intends to use the returned socket address structure in a call to the bind function.

			ready[0] = false;
			ready[1] = false;
			ready[2] = false;

			i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
			if (i_result != 0)
				throw std::runtime_error(("[TCP_server:error] WSAStartup failed with error " + std::to_string(i_result)).c_str());

			ready[0] = true;
		}
		/*
		WSAStartup: Initiates use of WS2_32.dll.
		WSADATA: Contains information about the Windows Sockets implementation.
		MAKEWORD: Requests version 2.2 of Winsock and sets it as the highest version of Windows Sockets support available to the caller.
		*/
	}

	void init(std::uint64_t port)
	{
		init();
		_bind(port);
		ready[1] = true;
	}

	void _bind(std::uint64_t port) // _ before bind because bind is already labeled by Microsoft
	{
		if (ready[1]) freeaddrinfo(result);
		if (c_port != port)
		{
			i_result = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result); // re-getaddrinfo cuz port may have changed

			if (i_result != 0)
			{
				WSACleanup();
				throw std::runtime_error(("[TCP_server:error] getaddrinfo failed with " + std::to_string(i_result)).c_str());
				return;
			}

			l_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			if (l_socket == INVALID_SOCKET)
			{
				freeaddrinfo(result);
				WSACleanup();
				throw std::runtime_error(("[TCP_server:error] Error at socket: " + std::to_string(WSAGetLastError())).c_str());
			}

			i_result = bind(l_socket, result->ai_addr, (int)result->ai_addrlen);

			if (i_result == SOCKET_ERROR)
			{
				freeaddrinfo(result);
				closesocket(l_socket);
				WSACleanup();
				throw std::runtime_error(("[TCP_server:error] Failed to bind with error " + std::to_string(WSAGetLastError())).c_str());
			}

			freeaddrinfo(result);
			c_port = port;
		}
	}

	void _listen()
	{
		if (ready[1])
		{
			if (listen(l_socket, SOMAXCONN) == SOCKET_ERROR)
			{
				std::cout << "[TCP_server:warning] Failed to listen. Throw runtime error? [Y/n]: ";
				std::string inp("");
				std::getline(std::cin, inp);

				if (inp.length() > 1) inp = " ";
				if (inp[0] == 'n') return;

				freeaddrinfo(result);
				closesocket(l_socket);
				WSACleanup();
				throw std::runtime_error(("[TCP_server:error] Failed to listen with error " + std::to_string(WSAGetLastError())).c_str());
			}
			else
			{
				std::cout << "[TCP_server:notification] Listening using TCP on localhost:" << c_port << ".\n";
				ready[2] = true;
			}
		}
	}

	void _accept(char* (*handle_input)(char*), bool* run, bool free_after)
	{
		if (ready[2])
		{
			do
			{
				c_socket = accept(l_socket, NULL, NULL);

				if (c_socket == INVALID_SOCKET)
				{
					freeaddrinfo(result);
					closesocket(l_socket);
					WSACleanup();
					throw std::runtime_error(("[TCP_server:error] Failed to accept with error " + std::to_string(WSAGetLastError())).c_str());
				}

				i_result = 1;
				i_result = recv(c_socket, recvbuff, recvbuflen, 0);
				if (i_result > 0)
				{
					std::cout << "[TCP_server:notification] Received successfully.\n";

					char* tmp = handle_input(recvbuff);
					for (std::uint64_t i = 0; i < recvbuflen; ++i) recvbuff[i] = tmp[i];
					recvbuff[recvbuflen - 1] = '\0';

					i_send_result = send(c_socket, tmp, i_result, 0);
					std::cout << "[TCP_server:notification] Sent successfully.\n";


					if (free_after) free(tmp);
					if (i_send_result == SOCKET_ERROR) std::cout << "[TCP_server:error] Failed to respond.\n";
				}
				else if (i_result < 0)
				{
					std::cout << "[TCP_server:error] recv failed with error " << WSAGetLastError() << '\n';
					closesocket(c_socket);
					WSACleanup();
				}
			} while (i_result > 0 || *run);
		}
	}

	void kill()
	{
		if (ready[0])
		{
			i_result = shutdown(c_socket, SD_SEND);
			if (i_result == SOCKET_ERROR)
			{
				closesocket(c_socket);
				WSACleanup();
				throw std::runtime_error(("[TCP_server:error] Failed to shutdown Client Socket with error" + std::to_string(WSAGetLastError())).c_str());
			}

			closesocket(c_socket);
			WSACleanup();
		}
	}

	~TCP_server() { kill(); }
};
