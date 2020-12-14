#include "Server.h"


Server::Server(void(*l_handler)(UINT32&, const PortNumber&, const PacketID&, Packet&, Server*))
	: m_running(false)
{
	// Bind a packet handler function.
	//InitializeCriticalSection(&m_mutex);
	m_packetHandler = std::bind(l_handler, 
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
		std::placeholders::_4, std::placeholders::_5);
}

Server::~Server(){ 
	Stop();
	//DeleteCriticalSection(&m_mutex);
}

void Server::BindTimeoutHandler(void(*l_handler)(const ClientID&)){
	m_timeoutHandler = std::bind(l_handler, std::placeholders::_1);
}

bool Server::Send(const ClientID& l_id, Packet& l_packet){
	//m_mutex.lock();
	//std::cout << "Server lock" << std::endl;
	auto itr = m_clients.find(l_id);
	if (itr == m_clients.end()){
		//m_mutex.unlock();
		return false; 
	}
	if (m_outgoing.send(l_packet, itr->second.m_clientIP, itr->second.m_clientPORT) 
		!= UdpSocket::Status::Done)
	{
		std::cout << "Error sending a packet..." << std::endl;
		//m_mutex.unlock();
		return false;
	}
	m_totalSent += l_packet.getDataSize();
	//std::cout << "Server unlock" << std::endl;
	//m_mutex.unlock();
	return true;
}

bool Server::Send(UINT32& l_ip, const PortNumber& l_port, Packet& l_packet){
	if (m_outgoing.send(l_packet, l_ip, l_port) != UdpSocket::Status::Done){ return false; }
	m_totalSent += l_packet.getDataSize();
	return true;
}

void Server::Broadcast(Packet& l_packet, const ClientID& l_ignore){
	//ttsv::Lock lock(&m_mutex);
	//std::cout << "Server lock" << std::endl;
	//m_mutex.lock();
	for (auto &client : m_clients){
		if (client.first == l_ignore){ continue; }
		if (m_outgoing.send(l_packet, client.second.m_clientIP, client.second.m_clientPORT)
			!= UdpSocket::Status::Done)
		{
			std::cout << "Error broadcasting a packet to client: " 
				<< client.first << std::endl;
			continue;
		}
		m_totalSent += l_packet.getDataSize();
	}
	//m_mutex.unlock();
	//LEAVE
}

void Server::Listen(){
	UINT32 ip;
	PortNumber port;
	Packet packet;
	std::cout << "Beginning to listen..." << std::endl;
	while (m_running){
		packet.clear();
		UdpSocket::Status status = m_incoming.receive(packet, ip, port);
		if (status != UdpSocket::Status::Done){
			if (m_running){
				std::cout << "Error receiving a packet from: "
					<< ip << ":" << port << std::endl;
				continue;
			} else {
				std::cout << "Socket unbound." << std::endl;
				break;
			}
		}

		m_totalReceived += packet.getDataSize();
		//Send(ip, port, packet);

		PacketID id;
		if (!(packet >> id)){
			std::cout << "Invalid packet received: unable to extract id." << std::endl;
			continue;
		} // Non-conventional packet.
		PacketType type = (PacketType)id;
		if ((id < (PacketID)PacketType::Disconnect) || (id >= (PacketID)PacketType::OutOfBounds)){
			std::cout << "Invalid packet received: id is out of bounds." << std::endl;
			continue;
		} // Invalid packet type.

		if (type == PacketType::Heartbeat){
			bool ClientFound = false;
			//std::cout << "Server lock" << std::endl;
			m_mutex.lock();
			for (auto &client : m_clients){
				auto& info = client.second;
				if (info.m_clientIP != ip || info.m_clientPORT != port){ continue; }
				ClientFound = true;
				if (!info.m_heartbeatWaiting){ std::cout << "Invalid heartbeat packet received!" << std::endl; break; }
				info.m_latency = m_serverTime - info.m_heartbeatSent;
				info.m_lastHeartbeat = m_serverTime;
				info.m_heartbeatWaiting = false;
				info.m_heartbeatRetry = 0;
				break;
			}
			m_mutex.unlock();
			if (!ClientFound){ std::cout << "Heartbeat from unknown client received..." << std::endl; }
			//LEAVE
		} else if (m_packetHandler){
			m_packetHandler(ip, port, id, packet, this); // Handle the packet.
		}
	}
	std::cout << "...Listening stopped." << std::endl;
}

void Server::Update(const sf::Time& l_time){
	m_serverTime += l_time;
	if (m_serverTime < 0){ 
		m_serverTime -= (int)Network::HighestTimestamp;
		//ttsv::Lock lock(&m_mutex);
		m_mutex.lock();
		//std::cout << "Server lock on update" << std::endl;
		for (auto &client : m_clients){
			client.second.m_lastHeartbeat = 
				std::abs(client.second.m_lastHeartbeat - (INT32)Network::HighestTimestamp);
		}
		m_mutex.unlock();
		//std::cout << "Server unlock on update" << std::endl;
	}

	//ttsv::Lock lock(&m_mutex);
	m_mutex.lock();

	//std::cout << "Server lock on update2" << std::endl;
	for (auto itr = m_clients.begin(); itr != m_clients.end();){
		INT32 elapsed = m_serverTime - itr->second.m_lastHeartbeat;
		if (elapsed >= HEARTBEAT_INTERVAL){
			if (elapsed >= (INT32)Network::ClientTimeout || itr->second.m_heartbeatRetry > HEARTBEAT_RETRIES){
				// Remove client.
				std::cout << "Client " << itr->first << " has timed out." << std::endl;
				if (m_timeoutHandler){ m_timeoutHandler(itr->first); }
				itr = m_clients.erase(itr);
				continue;
			}
			if (!itr->second.m_heartbeatWaiting || (elapsed >= HEARTBEAT_INTERVAL * (itr->second.m_heartbeatRetry + 1))){
				// Heartbeat
				if (itr->second.m_heartbeatRetry >= 3){
					std::cout << "Re-try(" << itr->second.m_heartbeatRetry
						<< ") heartbeat for client " << itr->first << std::endl;
				}
				Packet Heartbeat;
				StampPacket(PacketType::Heartbeat, Heartbeat);
				Heartbeat << (UINT32)m_serverTime;
				Send(itr->first, Heartbeat);
				if (itr->second.m_heartbeatRetry == 0){
					itr->second.m_heartbeatSent = m_serverTime;
				}
				itr->second.m_heartbeatWaiting = true;
				++itr->second.m_heartbeatRetry;

				m_totalSent += Heartbeat.getDataSize();
			}
		}
		++itr;
	}
	m_mutex.unlock();
	//std::cout << "Server unlock on update2" << std::endl;
	//LEAVE
}

ClientID Server::AddClient(const UINT32& l_ip, const PortNumber& l_port){
	//ttsv::Lock lock(&m_mutex);
	m_mutex.lock();
	std::cout << "Server lock on add client" << std::endl;
	for (auto &itr : m_clients){
		if (itr.second.m_clientIP == l_ip && itr.second.m_clientPORT == l_port){
			m_mutex.unlock();
			//LEAVE
			return ClientID(Network::NullID);
		}
	}
	ClientID id = m_lastID;
	ClientInfo info(l_ip, l_port, m_serverTime);
	m_clients.emplace(id, info);
	++m_lastID;
	std::cout << "Server unlock on add" << std::endl;
	m_mutex.unlock();
	//LEAVE
	return id;
}

ClientID Server::GetClientID(const UINT32& l_ip, const PortNumber& l_port){
	
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr){
		if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port){ return itr->first; }
	}
	return (ClientID)Network::NullID;
}

bool Server::HasClient(const ClientID& l_id){
	return (m_clients.find(l_id) != m_clients.end());
}

bool Server::HasClient(const UINT32& l_ip, const PortNumber& l_port){
	return(GetClientID(l_ip, l_port) >= 0);
}

bool Server::GetClientInfo(const ClientID& l_id, ClientInfo& l_info){
	//ttsv::Lock lock(&m_mutex);
	//std::cout << "Server lock" << std::endl;
	m_mutex.lock();
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr){
		if (itr->first == l_id){
			l_info = itr->second;
			//LEAVE
			m_mutex.unlock();
			return true;
		}
	}
	m_mutex.unlock();
	//LEAVE
	return false;
}

bool Server::RemoveClient(const ClientID& l_id){
//	ttsv::Lock lock(&m_mutex);
	//std::cout << "Server lock" << std::endl;
	m_mutex.lock();
	auto itr = m_clients.find(l_id);
	if (itr == m_clients.end()){
		//LEAVE
		m_mutex.unlock();
		return false; 
	}
	Packet p;
	StampPacket(PacketType::Disconnect, p);
	Send(l_id, p);
	m_clients.erase(itr);
	m_mutex.unlock();
	//LEAVE
	return true;
}

bool Server::RemoveClient(const UINT32& l_ip, const PortNumber& l_port){
	//ttsv::Lock lock(&m_mutex);
	//std::cout << "Server lock" << std::endl;
	m_mutex.lock();
	for (auto itr = m_clients.begin(); itr != m_clients.end(); ++itr)
	{
		if (itr->second.m_clientIP == l_ip && itr->second.m_clientPORT == l_port){
			Packet p;
			StampPacket(PacketType::Disconnect, p);
			Send(itr->first, p);
			std::cout << "Client leave: " << itr->first << std::endl;
			m_clients.erase(itr);
			m_mutex.unlock();
			//LEAVE
			return true;
		}
	}
	m_mutex.unlock();
	//LEAVE
	return false;
}

void Server::DisconnectAll(){
	if (!m_running){ return; }
	Packet p;
	StampPacket(PacketType::Disconnect, p);
	m_mutex.lock();
	Broadcast(p);
	m_clients.clear();
	m_mutex.unlock();
}

bool Server::Start(){
	if (m_running){ return false; }
	if (m_incoming.bind((unsigned short)Network::ServerPort) != UdpSocket::Status::Done){ return false; }
	m_outgoing.bind(0);
	Setup();
	std::cout << "Incoming port: " <<
		m_incoming.getLocalPort() << ". Outgoing port: " 
		<< m_outgoing.getLocalPort() << std::endl;
	m_running = true;
	m_listenThread = std::thread(&Server::Listen, this);
	m_listenThread.detach();
	return true;
}

bool Server::Stop(){
	if (!m_running){ return false; }
	DisconnectAll();
	m_running = false;
	m_incoming.unbind();
	return true;
}

bool Server::IsRunning(){ return m_running; }

unsigned int Server::GetClientCount(){ return m_clients.size(); }
std::string Server::GetClientList(){
	std::string list;
	std::string delimiter = "--------------------------------------";
	list = delimiter;
	list += '\n';
	list += "ID";
	list += '\t';
	list += "Client IP:PORT";
	list += '\t'; list += '\t';
	list += "Ping";
	list += '\n';
	list += delimiter;
	list += '\n';
	for (auto &client : m_clients){
		list += std::to_string(client.first); 
		list += '\t';
		list += client.second.m_clientIP + ":" + std::to_string(client.second.m_clientPORT);
		list += '\t'; list += '\t';
		list += std::to_string(client.second.m_latency) + "ms.";
		list += '\n';
	}
	list += delimiter;
	list += '\n';
	list += "Total data sent: " + std::to_string(m_totalSent / 1000) + "kB. Total data received: " + std::to_string(m_totalReceived / 1000) + "kB";
	return list;
}

void Server::Lock(){ m_mutex.lock(); }

void Server::Unlock()
{
	m_mutex.unlock();
}

void Server::Setup(){
	m_lastID = 0;
	m_running = false;
	m_totalSent = 0;
	m_totalReceived = 0;
}