#include "Client.h"
#include <ws2tcpip.h>

Client::Client()
{
	InitializeCriticalSection(&m_mutex);
}
Client::~Client()
{ 
	m_socket.unbind();
	DeleteCriticalSection(&m_mutex);
}

bool Client::Connect(){
	if (m_connected){ return false; }
	m_socket.bind(0);
	std::cout << "Bound client to port: " << m_socket.getLocalPort() << std::endl;
	Packet p;
	StampPacket(PacketType::Connect, p);
	p << m_playerName;
	if (m_socket.send(p, m_serverIp, m_serverPort) != UdpSocket::Status::Done){ m_socket.unbind(); return false; }
	m_socket.setBlocking(false);
	p.clear();
	UINT recvIP;
	PortNumber recvPORT;
	Clock timer;
	timer.restart();

	std::cout << "Attempting to connect to: " << m_serverIp << ":" << m_serverPort << std::endl;
	while (timer.getElapsedTime() < CONNECT_TIMEOUT){
		UdpSocket::Status s = m_socket.receive(p, recvIP, recvPORT);
		if (s != UdpSocket::Status::Done){ continue; }
		if (recvIP != m_serverIp){ continue; }
		PacketID id;
		if (!(p >> id)){ break; }
		if (id != PacketID(PacketType::Connect)){ continue; }
		//m_socket.setBlocking(true);
		m_packetHandler(id, p, this);
		m_connected = true;
		m_socket.setBlocking(true);
		m_lastHeartbeat = m_serverTime;
		m_listenThread = std::thread(&Client::Listen, this);
		m_listenThread.detach();
		return true;
	}
	std::cout << "Connection attempt failed! Server info: " << m_serverIp << ":" << m_serverPort << std::endl;
	m_socket.unbind();
	m_socket.setBlocking(true);
	return false;
}

bool Client::Disconnect(){
	if (!m_connected){ return false; }
	Packet p;
	StampPacket(PacketType::Disconnect, p);
	UdpSocket::Status s = m_socket.send(p, m_serverIp, m_serverPort);
	m_connected = false;
	m_socket.unbind(); // Unbind to close the listening thread.
	if (s != UdpSocket::Status::Done){ return false; }
	return true;
}

void Client::Listen(){
	Packet packet;
	UINT recvIP;
	PortNumber recvPORT;
	std::cout << "Beginning to listen..." << std::endl;
	while (m_connected){
		packet.clear();
		UdpSocket::Status status = m_socket.receive(packet, recvIP, recvPORT);
		if (status != UdpSocket::Status::Done){
			if (m_connected){
				std::cout << "Failed receiving a packet from "
					<< recvIP << ":" << recvPORT  << std::endl;
				continue;
			} else {
				std::cout << "Socket unbound." << std::endl;
				break;
			}
		}
		if (recvIP != m_serverIp){
			// Ignore packets not sent from the server.
			std::cout << "Invalid packet received: invalid origin." << std::endl;
			continue;
		}
		PacketID id;
		if (!(packet >> id)){
			// Non-conventional packet.
			std::cout << "Invalid packet received: unable to extract id." << std::endl;
			continue;
		}
		PacketType type = (PacketType)id;
		if (type < PacketType::Disconnect || type >= PacketType::OutOfBounds){
			// Invalid packet type.
			std::cout << "Invalid packet received: id is out of bounds." << std::endl;
			continue;
		}

		if (type == PacketType::Heartbeat){
			Packet p;
			StampPacket(PacketType::Heartbeat, p);
			if (m_socket.send(p, m_serverIp, m_serverPort) != UdpSocket::Status::Done){
				std::cout << "Failed sending a heartbeat!" << std::endl;
			}
			INT32 timestamp;
			packet >> timestamp;
			SetTime(timestamp);
			m_lastHeartbeat = m_serverTime;
		} else if(m_packetHandler){
			m_packetHandler(id, packet, this); // Handle the packet.
		}
	}
	std::cout << "...Listening stopped." << std::endl;
}

bool Client::Send(Packet& l_packet){
	if (!m_connected){ return false; }
	if (m_socket.send(l_packet, m_serverIp, m_serverPort) != UdpSocket::Status::Done){ return false; }
	return true;
}

const sf::Time& Client::GetTime()const{
	return m_serverTime;
}

const sf::Time& Client::GetLastHeartbeat()const{
	return m_lastHeartbeat;
}

void Client::SetTime(const sf::Time& l_time){
	m_serverTime = l_time;
}

void Client::SetServerInformation(const char* l_ip, const PortNumber& l_port){
	inet_pton(AF_INET, l_ip, &m_serverIp);
	m_serverIp = ntohl(m_serverIp);
	m_serverPort = l_port;
}

void Client::Update(const sf::Time& l_time){
	if (!m_connected){ return; }
	m_serverTime += l_time;
	if (m_serverTime < 0){
		m_serverTime -= (INT32)Network::HighestTimestamp;
		m_lastHeartbeat = m_serverTime;
		return;
	}
	if (m_serverTime - m_lastHeartbeat >= (INT32)Network::ClientTimeout){
		// Timeout.
		std::cout << "Server connection timed out!" << std::endl;
		Disconnect();
	}
}

bool Client::IsConnected()const{ return m_connected; }
void Client::SetPlayerName(const std::string& l_name){ m_playerName = l_name; }

CRITICAL_SECTION& Client::GetMutex(){ return m_mutex; }

void Client::Setup(void(*l_handler)(const PacketID&, Packet&, Client*)){
	m_packetHandler = std::bind(l_handler,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void Client::UnregisterPacketHandler(){
	m_packetHandler = nullptr;
}