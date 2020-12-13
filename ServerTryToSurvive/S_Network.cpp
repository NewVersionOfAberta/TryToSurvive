#include "S_Network.h"
#include "System_Manager.h"
#include "Lock.h"


S_Network::S_Network(SystemManager* l_systemMgr)
	: S_Base(System::Network, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Client);
	m_requiredComponents.push_back(req);

	MessageHandler* messageHandler = 
		m_systemManager->GetMessageHandler();
	messageHandler->Subscribe(EntityMessage::Removed_Entity, this);
	messageHandler->Subscribe(EntityMessage::Hurt, this);
	messageHandler->Subscribe(EntityMessage::Respawn, this);
}

S_Network::~S_Network(){}

void S_Network::Update(float l_dT){
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		auto& player = m_playerInput[entity];
		if (player.m_movedX || player.m_movedY){
			if (player.m_movedX){
				Message msg((MessageType)EntityMessage::Move);
				msg.m_receiver = entity;
				if (player.m_movedX > 0){ msg.m_int = (int)Direction::Right; }
				else { msg.m_int = (int)Direction::Left; }
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			}

			if (player.m_movedY){
				Message msg((MessageType)EntityMessage::Move);
				msg.m_receiver = entity;
				if (player.m_movedY > 0){ msg.m_int = (int)Direction::Down; }
				else { msg.m_int = (int)Direction::Up; }
				m_systemManager->GetMessageHandler()->Dispatch(msg);
			}
		}
		if (player.m_x > 0.0 || player.m_y > 0.0){
			Message msg((MessageType)EntityMessage::Attack);
			msg.m_2f.m_x = player.m_x;
			msg.m_2f.m_y = player.m_y;
			m_systemManager->GetMessageHandler()->Dispatch(msg);
		}
	}
}

void S_Network::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event){}

void S_Network::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver)){ return; }
	EntityMessage m = EntityMessage(l_message.m_type);
	if (m == EntityMessage::Removed_Entity){ m_playerInput.erase(l_message.m_receiver); return; }
	if (m == EntityMessage::Hurt){
		Packet packet;
		StampPacket(PacketType::Hurt, packet);
		packet << l_message.m_receiver;
		m_server->Broadcast(packet);
		return;
	}
	if (m == EntityMessage::Respawn){
		C_Position* position = m_systemManager->GetEntityManager()->GetComponent<C_Position>(l_message.m_receiver, Component::Position);
		if (!position){ return; }
		position->SetPosition(64.f, 64.f);
		position->SetElevation(1);
	}
}

bool S_Network::RegisterClientID(const EntityId& l_entity, const ClientID& l_client){
	if (!HasEntity(l_entity)){ return false; }
	m_systemManager->GetEntityManager()->GetComponent<C_Client>
		(l_entity, Component::Client)->SetClientID(l_client);
	return true;
}

void S_Network::RegisterServer(Server* l_server){ m_server = l_server; }

ClientID S_Network::GetClientID(const EntityId& l_entity){
	if (!HasEntity(l_entity)){ return (ClientID)Network::NullID; }
	return m_systemManager->GetEntityManager()->GetComponent<C_Client>(l_entity, Component::Client)->GetClientID();
}

EntityId S_Network::GetEntityID(const ClientID& l_client){
	EntityManager* e = m_systemManager->GetEntityManager();
	auto entity = std::find_if(m_entities.begin(), m_entities.end(),
		[&e, &l_client](EntityId& id){
			return e->GetComponent<C_Client>(id, Component::Client)->GetClientID() == l_client;
	});
	return(entity != m_entities.end() ? *entity : (EntityId)Network::NullID);
}

void S_Network::CreateSnapshot(Packet& l_packet){
	/*std::mutex* l_mutex = m_server->GetMutex();
	ttsv::Lock lock(l_mutex);*/
	m_server->Lock();
	ServerEntityManager* e = (ServerEntityManager*)m_systemManager->GetEntityManager();
	StampPacket(PacketType::Snapshot, l_packet);
	l_packet << INT32(e->GetEntityCount());
	if (e->GetEntityCount()){
		e->DumpEntityInfo(l_packet);
	}
	m_server->Unlock();
	//LEAVE
}

void S_Network::UpdatePlayer(Packet& l_packet, const ClientID& l_cid){
	/*std::mutex* l_mutex = m_server->GetMutex();
	ttsv::Lock lock(l_mutex);*/
	m_server->Lock();
	EntityId eid = GetEntityID(l_cid);
	if (eid == -1){ 
		m_server->Unlock();
		//LEAVE
		return; 
	}
	if (!HasEntity(eid)){
		m_server->Unlock();
		//LEAVE
		return;
	}
	INT32 entity_message;
	while (l_packet >> entity_message){
		switch (entity_message){
		case (INT32)EntityMessage::Move:
		{
			INT32 x = 0, y = 0;
			l_packet >> x >> y;
			//std::cout << "Player " <<  eid <<" moved on " << x << " : " << y << std::endl;
			m_playerInput[eid].m_movedX = x;
			m_playerInput[eid].m_movedY = y;
			break;
		}
		case (INT32)EntityMessage::Attack:
		{
			float m_x, m_y;
			l_packet >> m_x >> m_y;
			m_playerInput[eid].m_x = m_x;
			m_playerInput[eid].m_y = m_y;
			break;
		}
		}
		INT32 delim = 0;
		if (!(l_packet >> delim) || delim != (INT32)Network::PlayerUpdateDelim){
			std::cout << "Faulty update!" << std::endl; 
			break;
		}
	}
	m_server->Unlock();
	//LEAVE
}