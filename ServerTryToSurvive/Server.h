#pragma once

#include "PacketTypes.h"
#include "NetworkDefinitions.h"
#include "UDPSocket.h"
#include "Lock.h"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>

#define HEARTBEAT_INTERVAL 1000 // ms.
#define HEARTBEAT_RETRIES 25

struct ClientInfo{
	ClientInfo(const UINT& l_ip, const PortNumber& l_port,
		const sf::Time& l_heartbeat): m_clientIP(l_ip), m_clientPORT(l_port), 
		m_lastHeartbeat(l_heartbeat), m_heartbeatWaiting(false), m_heartbeatRetry(0), m_latency(0)
	{}

	ClientInfo& operator=(const ClientInfo& l_rhs){
		m_clientIP			= l_rhs.m_clientIP;
		m_clientPORT		= l_rhs.m_clientPORT;
		m_lastHeartbeat		= l_rhs.m_lastHeartbeat;
		m_heartbeatSent		= l_rhs.m_heartbeatSent;
		m_heartbeatWaiting	= l_rhs.m_heartbeatWaiting;
		m_heartbeatRetry	= l_rhs.m_heartbeatRetry;
		m_latency			= l_rhs.m_latency;
		return *this;
	}
	UINT32 m_clientIP;
	PortNumber m_clientPORT;
	sf::Time m_lastHeartbeat;
	sf::Time m_heartbeatSent;
	bool m_heartbeatWaiting;
	unsigned short m_heartbeatRetry;
	unsigned int m_latency;
};

using Clients = std::unordered_map<ClientID, ClientInfo>;
class Server;
using PacketHandler = std::function<void(UINT32&, const PortNumber&, const PacketID&, Packet&, Server*)>;
using TimeoutHandler = std::function<void(const ClientID&)>;

class Server{
public:
	template <class T>
	Server(void(T::*l_handler)(UINT32&, const PortNumber&, const PacketID&, Packet&, Server*),
		T* l_instance) : m_running(false)
	{
		//InitializeCriticalSection(&m_mutex);
		m_packetHandler = std::bind(l_handler, l_instance,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::placeholders::_4, std::placeholders::_5);
	}

	Server(void(*l_handler)(UINT32&, const PortNumber&, const PacketID&, Packet&, Server*));
	~Server();

	template<class T>
	void BindTimeoutHandler(void(T::*l_handler)(const ClientID&), T* l_instance){
		m_timeoutHandler = std::bind(l_handler, l_instance, std::placeholders::_1);
	}
	void BindTimeoutHandler(void(*l_handler)(const ClientID&));

	bool Send(const ClientID& l_id, Packet& l_packet);
	bool Send(UINT32& l_ip, const PortNumber& l_port, Packet& l_packet);
	void Broadcast(Packet& l_packet, const ClientID& l_ignore = (ClientID)Network::NullID);

	void Listen();
	void Update(const sf::Time& l_time);

	ClientID AddClient(const UINT32& l_ip, const PortNumber& l_port);
	ClientID GetClientID(const UINT32& l_ip, const PortNumber& l_port);
	bool HasClient(const ClientID& l_id);
	bool HasClient(const UINT32& l_ip, const PortNumber& l_port);
	bool GetClientInfo(const ClientID& l_id, ClientInfo& l_info);
	bool RemoveClient(const ClientID& l_id);
	bool RemoveClient(const UINT32& l_ip, const PortNumber& l_port);

	void DisconnectAll();
	bool Start();
	bool Stop();

	bool IsRunning();

	unsigned int GetClientCount();
	std::string GetClientList();

	void Lock();
	void Unlock();
private:
	void Setup();
	ClientID m_lastID;

	UdpSocket m_incoming;
	UdpSocket m_outgoing;

	PacketHandler m_packetHandler;
	TimeoutHandler m_timeoutHandler;

	Clients m_clients;
	sf::Time m_serverTime;

	bool m_running;

	std::thread m_listenThread;
	std::mutex m_mutex;

	size_t m_totalSent;
	size_t m_totalReceived;
};