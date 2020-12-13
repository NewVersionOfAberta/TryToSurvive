//#include <string>

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Packet.h"

    Packet::Packet() :
        m_readPos(0),
        m_sendPos(0),
        m_isValid(true)
    {

    }


    Packet::~Packet()
    {

    }


    void Packet::append(const void* data, std::size_t sizeInBytes)
    {
        if (data && (sizeInBytes > 0))
        {
            std::size_t start = m_data.size();
            m_data.resize(start + sizeInBytes);
            std::memcpy(&m_data[start], data, sizeInBytes);
        }
    }


    std::size_t Packet::getReadPosition() const
    {
        return m_readPos;
    }


    void Packet::clear()
    {
        m_data.clear();
        m_readPos = 0;
        m_isValid = true;
    }


    const void* Packet::getData() const
    {
        return !m_data.empty() ? &m_data[0] : NULL;
    }

    std::size_t Packet::getDataSize() const
    {
        return m_data.size();
    }


    bool Packet::endOfPacket() const
    {
        return m_readPos >= m_data.size();
    }

    Packet::operator BoolType() const
    {
        return m_isValid ? &Packet::checkSize : NULL;
    }

    Packet& Packet::operator>>(UINT8& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = *reinterpret_cast<const UINT8*>(&m_data[m_readPos]);
            m_readPos += sizeof(data);
        }
        return *this;
    }

    Packet& Packet::operator>>(INT8& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = *reinterpret_cast<const INT8*>(&m_data[m_readPos]);
            m_readPos += sizeof(data);
        }
        return *this;
    }


    Packet& Packet::operator >>(bool& data)
    {
        UINT8 value;
        if (*this >> value)
            data = (value != 0);

        return *this;
    }

    Packet& Packet::operator >>(INT32& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = ntohl(*reinterpret_cast<const INT32*>(&m_data[m_readPos]));
            m_readPos += sizeof(data);
        }

        return *this;
    }

    Packet& Packet::operator >>(sf::Uint32& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = ntohl(*reinterpret_cast<const sf::Uint32*>(&m_data[m_readPos]));
            m_readPos += sizeof(data);
        }

        return *this;
    }


    Packet& Packet::operator >>(float& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = *reinterpret_cast<const float*>(&m_data[m_readPos]);
            m_readPos += sizeof(data);
        }

        return *this;
    }

    Packet& Packet::operator >>(double& data)
    {
        if (checkSize(sizeof(data)))
        {
            data = *reinterpret_cast<const double*>(&m_data[m_readPos]);
            m_readPos += sizeof(data);
        }

        return *this;
    }

    Packet& Packet::operator >>(char* data)
    {
        // First extract string length
        sf::Uint32 length = 0;
        *this >> length;

        if ((length > 0) && checkSize(length))
        {
            // Then extract characters
            std::memcpy(data, &m_data[m_readPos], length);
            data[length] = '\0';

            // Update reading position
            m_readPos += length;
        }

        return *this;
    }


    Packet& Packet::operator >>(std::string& data)
    {
        // First extract string length
        sf::Uint32 length = 0;
        *this >> length;

        data.clear();
        if ((length > 0) && checkSize(length))
        {
            // Then extract characters
            data.assign(&m_data[m_readPos], length);

            // Update reading position
            m_readPos += length;
        }

        return *this;
    }


    Packet& Packet::operator <<(bool data)
    {
        *this << static_cast<UINT8>(data);
        return *this;
    }

    Packet& Packet::operator <<(INT32 data)
    {
        INT32 toWrite = htonl(data);
        append(&toWrite, sizeof(toWrite));
        return *this;
    }


    Packet& Packet::operator <<(sf::Uint32 data)
    {
        sf::Uint32 toWrite = htonl(data);
        append(&toWrite, sizeof(toWrite));
        return *this;
    }

    
    Packet& Packet::operator <<(float data)
    {
        append(&data, sizeof(data));
        return *this;
    }


    Packet& Packet::operator <<(double data)
    {
        append(&data, sizeof(data));
        return *this;
    }


    Packet& Packet::operator <<(const char* data)
    {
        // First insert string length
        sf::Uint32 length = static_cast<sf::Uint32>(std::strlen(data));
        *this << length;

        // Then insert characters
        append(data, length * sizeof(char));

        return *this;
    }


    Packet& Packet::operator <<(const std::string& data)
    {
        // First insert string length
        sf::Uint32 length = static_cast<sf::Uint32>(data.size());
        *this << length;

        // Then insert characters
        if (length > 0)
            append(data.c_str(), length * sizeof(std::string::value_type));

        return *this;
    }


    bool Packet::checkSize(std::size_t size)
    {
        m_isValid = m_isValid && (m_readPos + size <= m_data.size());

        return m_isValid;
    }


    const void* Packet::onSend(std::size_t& size)
    {
        size = getDataSize();
        return getData();
    }


    void Packet::onReceive(const void* data, std::size_t size)
    {
        append(data, size);
    }

