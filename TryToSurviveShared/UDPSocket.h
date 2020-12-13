#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#include "Packet.h"


#define DEFAULT_PORT 27015

class UdpSocket
{
public:
    enum class Status
    {
        Done,         //!< The socket has sent / received the data
        NotReady,     //!< The socket is not ready to send / receive data yet
        Partial,      //!< The socket sent a part of the data
        Disconnected, //!< The TCP socket has been disconnected
        Error         //!< An unexpected error happened
    };

    UdpSocket();
    ~UdpSocket();

    //Status receive(void* data, std::size_t size, std::size_t& received, UINT32& remoteAddress, unsigned short& remotePort);
    Status receive(Packet& packet, UINT32& remoteAddress, unsigned short& remotePort);
    Status send(Packet& packet, const UINT32& remoteAddress, unsigned short remotePort);
    void create();
    void close();
    Status bind(unsigned short port, const UINT32& address = 0);

    unsigned short getLocalPort() const;

    void setBlocking(bool blocking);
    void unbind();

private:

    sockaddr_in createAddress(UINT32 address, unsigned short port);


    UdpSocket::Status getErrorStatus();
    std::vector<char> m_buffer;
    bool m_isBlocking;
    SOCKET m_socket;

    //

};

