#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>

////
#include <string>
#include "Usings.h"
//
#pragma comment (lib, "Ws2_32.lib")
//
class Packet
{
public:
    typedef bool (Packet::* BoolType)(std::size_t);


    Packet();

    virtual ~Packet();

    operator BoolType() const;

    void append(const void* data, std::size_t sizeInBytes);

    std::size_t getReadPosition() const;

    void clear();

    const void* getData() const;


    std::size_t getDataSize() const;

    bool endOfPacket() const;

    Packet& operator >>(bool&         data);

    Packet& operator >>(INT32&        data);

    Packet& operator >>(sf::Uint32&       data);

    Packet& operator >>(float&        data);

    Packet& operator >>(double&       data);

    Packet& operator >>(char*         data);

    Packet& operator >>(std::string&  data);

    Packet& operator >>(UINT8&        data);

    Packet& operator >>(INT8& data);

    Packet& operator <<(bool                data);


    Packet& operator <<(INT32               data);

    Packet& operator <<(sf::Uint32              data);

    Packet& operator <<(float               data);

    Packet& operator <<(double              data);

    Packet& operator <<(const char*         data);

    Packet& operator <<(const std::string&  data);

protected:

    friend class UdpSocket;

    virtual const void* onSend(std::size_t& size);

    virtual void onReceive(const void* data, std::size_t size);

private:

    bool checkSize(std::size_t size);

    std::vector<char> m_data;    //!< Data stored in the packet
    int       m_readPos; //!< Current reading position in the packet
    int       m_sendPos; //!< Current send position in the packet (for handling partial sends)
    bool              m_isValid; //!< Reading state of the packet
};
 