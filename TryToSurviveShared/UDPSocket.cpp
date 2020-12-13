#include "UDPSocket.h"

#include <iostream>
#include <ws2tcpip.h>

#define MaxDatagramSize 65507


UdpSocket::UdpSocket(): m_buffer(MaxDatagramSize), m_isBlocking(false)
{
}

UdpSocket::~UdpSocket()
{
	close();
}



void UdpSocket::close()
{
	// Close the socket
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

UdpSocket::Status UdpSocket::bind(unsigned short port, const UINT32& address)
{
	// Close the socket if it is already bound
	close();

	// Create the internal socket if it doesn't exist
	create();

	// Bind the socket
	sockaddr_in addr = createAddress(address, port);
	if (::bind(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
	{
		std::cout << "Failed to bind socket to port " << port << std::endl;
		return UdpSocket::Status::Error;
	}

	return UdpSocket::Status::Done;
}

unsigned short UdpSocket::getLocalPort() const
{
	if (m_socket != INVALID_SOCKET)
	{
		// Retrieve informations about the local end of the socket
		sockaddr_in address;
		int size = sizeof(address);
		if (getsockname(m_socket, reinterpret_cast<sockaddr*>(&address), &size) != -1)
		{
			return ntohs(address.sin_port);
		}
	}

	// We failed to retrieve the port
	return 0;
}

void UdpSocket::unbind()
{
	// Simply close the socket
	close();
}

sockaddr_in UdpSocket::createAddress(UINT32 address, unsigned short port)
{
	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = htonl(address);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	return addr;
}

void UdpSocket::setBlocking(bool blocking)
{
	// Apply if the socket is already created
	if (m_socket != INVALID_SOCKET) {
		u_long block = blocking ? 0 : 1;
		ioctlsocket(m_socket, FIONBIO, &block);
	}

	m_isBlocking = blocking;
}

UdpSocket::Status UdpSocket::getErrorStatus()
{
	int error = WSAGetLastError();
	switch (error)
	{
	case WSAEWOULDBLOCK:  return UdpSocket::Status::NotReady;
	case WSAEALREADY:     return UdpSocket::Status::NotReady;
	case WSAECONNABORTED: return UdpSocket::Status::Disconnected;
	case WSAECONNRESET:   return UdpSocket::Status::Disconnected;
	case WSAETIMEDOUT:    return UdpSocket::Status::Disconnected;
	case WSAENETRESET:    return UdpSocket::Status::Disconnected;
	case WSAENOTCONN:     return UdpSocket::Status::Disconnected;
	case WSAEISCONN:      return UdpSocket::Status::Done;
	default:              return UdpSocket::Status::Error;
	}
}


UdpSocket::Status UdpSocket::receive(Packet& packet, UINT32& remoteAddress, unsigned short& remotePort)
{
	// First clear the variables to fill
	int received = 0;
	remoteAddress = 0;
	remotePort = 0;
	
	void* data = &m_buffer[0];
	size_t size = m_buffer.size();
	// Check the destination buffer
	if (!data)
	{
		std::cout << "Cannot receive data from the network (the destination buffer is invalid)" << std::endl;
		return Status::Error;
	}

	// Data that will be filled with the other computer's address
	sockaddr_in address = createAddress(INADDR_ANY, 0);

	// Receive a chunk of bytes
	int addressSize = sizeof(address);
	int sizeReceived;
	for (int i = 0; i < 5; i++) {
		sizeReceived = recvfrom(m_socket, static_cast<char*>(data), static_cast<int>(size), 0, reinterpret_cast<sockaddr*>(&address), &addressSize);
		if (sizeReceived < 0) {
			UdpSocket::Status status = getErrorStatus();
			if (status != UdpSocket::Status::NotReady) {
				return status;
			}
			Sleep(50);
		}
		else {
			break;
		}
		
	}
	// Check for errors
	if (sizeReceived < 0)
		return getErrorStatus();

	// Fill the sender informations
	received = static_cast<std::size_t>(sizeReceived);
	remoteAddress = ntohl(address.sin_addr.s_addr);
	remotePort = ntohs(address.sin_port);
	packet.clear();
	if (received > 0)
		packet.onReceive(&m_buffer[0], received);

	return UdpSocket::Status::Done;
}


UdpSocket::Status UdpSocket::send(Packet& packet, const UINT32& remoteAddress, unsigned short remotePort)
{
	std::size_t size = 0;
	const void* data = packet.onSend(size);
	// Create the internal socket if it doesn't exist
	create();

	// Make sure that all the data will fit in one datagram
	if (size > MaxDatagramSize)
	{
		std::cout << "Cannot send data over the network "
			<< "(the number of bytes to send is greater than sf::UdpSocket::MaxDatagramSize)" << std::endl;
		return UdpSocket::Status::Error;
	}

	// Build the target address
	sockaddr_in address = createAddress(remoteAddress, remotePort);
	//inet_pton(AF_INET, "127.0.0.256", &address.sin_addr.s_addr);
	
	// Send the data (unlike TCP, all the data is always sent in one call)
	int sent = sendto(m_socket, static_cast<const char*>(data), 
		static_cast<int>(size), 0, reinterpret_cast<sockaddr*>(&address), sizeof(address));

	// Check for errors
	if (sent < 0)
		return getErrorStatus();

	return UdpSocket::Status::Done;
}

void UdpSocket::create()
{
	if (m_socket == INVALID_SOCKET)
	{
		SOCKET handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (handle == INVALID_SOCKET)
		{
			std::cout << "Failed to create socket" << std::endl;
			return;
		}
		m_socket = handle;
		int yes = 1;
		if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
		{
			std::cout << "Failed to enable broadcast on UDP socket" << std::endl;
		}
	}
}


struct SocketInitializer
{
	SocketInitializer()
	{
		WSADATA init;
		WSAStartup(MAKEWORD(2, 2), &init);
	}

	~SocketInitializer()
	{
		WSACleanup();
	}
};

SocketInitializer globalInitializer;