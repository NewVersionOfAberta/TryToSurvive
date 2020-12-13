#pragma once
#include <functional>
#include <iostream>
#include <thread>
#include "NetworkDefinitions.h"
#include "PacketTypes.h"
#include "NetSettings.h"
#include "UDPSocket.h"
#include "Clock.h"

#define CONNECT_TIMEOUT 500000 // ms

class Client;
using PacketHandler = std::function<void(const PacketID&, Packet&, Client*)>;

class Client{
public:
	Client();
	~Client();

	bool Connect();
	bool Disconnect();

	void Listen();
	bool Send(Packet& l_packet);

	const sf::Time& GetTime() const;
	const sf::Time& GetLastHeartbeat() const;
	void SetTime(const sf::Time& l_time);
	void SetServerInformation(const char* l_ip, const PortNumber& l_port);


	template<class T>
	void Setup(void(T::*l_handler)(const PacketID&, Packet&, Client*), T* l_instance){
		m_packetHandler = std::bind(l_handler, l_instance,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}
	void Setup(void(*l_handler)(const PacketID&, Packet&, Client*));
	void UnregisterPacketHandler();

	void Update(const sf::Time& l_time);

	bool IsConnected() const;
	void SetPlayerName(const std::string& l_name);

	CRITICAL_SECTION& GetMutex();
private:
	std::string m_playerName;

	UdpSocket m_socket;
	UINT m_serverIp;
	PortNumber m_serverPort;
	PacketHandler m_packetHandler;
	bool m_connected;
	sf::Time m_serverTime;
	sf::Time m_lastHeartbeat;

	std::thread m_listenThread;
	CRITICAL_SECTION m_mutex;
};