#include "S_Network.h"
#include "System_Manager.h"
#include "S_Renderer.h"
#include "Lock.h"



S_Network::S_Network(SystemManager* l_systemMgr)
	: S_Base(System::Network, l_systemMgr), m_client(nullptr), m_player(0)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Client);
	m_requiredComponents.push_back(req);

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Bullet_add, this);
	m_playerUpdateTimer = 0;
}


void InterpolateSnapshot(const EntitySnapshot& l_s1, const INT32& T1,
	const EntitySnapshot& l_s2, const INT32& T2, EntitySnapshot& l_target, const INT32& T_X)
{
	l_target.m_direction = l_s2.m_direction;
	l_target.m_state = l_s1.m_state;
	l_target.m_health = l_s2.m_health;
	l_target.m_name = l_s2.m_name;
	l_target.m_elevation = l_s1.m_elevation;

	l_target.m_position.first = Interpolate<float>(
		T1, T2, l_s1.m_position.first, l_s2.m_position.first, T_X);
	l_target.m_position.second = Interpolate<float>(
		T1, T2, l_s1.m_position.second, l_s2.m_position.second, T_X);

	l_target.m_velocity.first = Interpolate<float>(
		T1, T2, l_s1.m_velocity.first, l_s2.m_velocity.first, T_X);
	l_target.m_velocity.second = Interpolate<float>(
		T1, T2, l_s1.m_velocity.second, l_s2.m_velocity.second, T_X);

	l_target.m_acceleration.first = Interpolate<float>(
		T1, T2, l_s1.m_acceleration.first, l_s2.m_acceleration.first, T_X);
	l_target.m_acceleration.second = Interpolate<float>(
		T1, T2, l_s1.m_acceleration.second, l_s2.m_acceleration.second, T_X);

}

bool CompareSnapshots(const EntitySnapshot& l_s1, const EntitySnapshot& l_s2,
	bool l_position, bool l_physics, bool l_state)
{
	if (l_position && (l_s1.m_position != l_s2.m_position || 
		l_s1.m_elevation != l_s2.m_elevation))
	{ return false; }
	if (l_physics && (l_s1.m_velocity != l_s2.m_velocity ||
		l_s1.m_acceleration != l_s2.m_acceleration ||
		l_s1.m_direction != l_s2.m_direction))
	{ return false; }
	if (l_state && (l_s1.m_state != l_s2.m_state))
	{ return false; }
	return true;
}


S_Network::~S_Network(){}

void S_Network::Update(float l_dT){
	if (!m_client){ return; }
	//m_client->Lock();
	//std::cout << "S_Network::Update. Take lock at: " << clock.getCurrentTime() << std::endl;
	m_playerUpdateTimer += l_dT;
	//std::cout << "Time (sec): " << m_playerUpdateTimer << " cur time (mills) " << l_dT << std::endl;
	if (Clock::secondsAsMills(m_playerUpdateTimer) >= PLAYER_UPDATE_INTERVAL) {
		SendPlayerOutgoing(l_dT);
		m_playerUpdateTimer = 0;
	}
	//std::cout << "S_Network::Update. Leave at: " << clock.getCurrentTime() << std::endl;
	PerformInterpolation();
	//m_client->Unlock();
	//LEAVE
}

void S_Network::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event){
}

void S_Network::Notify(const Message& l_message){
	if (!HasEntity(l_message.m_receiver) || l_message.m_receiver != m_player){ 
		return;
	}
	//if (l_message.m_type == (MessageType)EntityMessage::Attack && m_outgoing.find(EntityMessage::Attack) != m_outgoing.end()){ return; }
	m_outgoing[(EntityMessage)l_message.m_type].emplace_back(l_message);
}

void S_Network::SetClient(Client* l_client){ m_client = l_client; }

void S_Network::SetPlayerID(const EntityId& l_entity){ m_player = l_entity; }

void S_Network::AddSnapshot(const EntityId& l_entity, 
	const INT32& l_timestamp, EntitySnapshot& l_snapshot)
{
	//m_client->Lock();
	//std::cout << "S_Network::AddSnapshot. Take lock at: " << clock.getCurrentTime() << std::endl;
	auto i = m_entitySnapshots.emplace(l_timestamp, SnapshotDetails());
	i.first->second.m_snapshots.emplace(l_entity, l_snapshot);
	//std::cout << "S_Network::AddSnapshot. Leave at: " << clock.getCurrentTime() << std::endl;
	//m_client->Unlock();
	//LEAVE
}

void S_Network::ApplyEntitySnapshot(const EntityId& l_entity, 
	const EntitySnapshot& l_snapshot, bool l_applyPhysics)
{
	ClientEntityManager* entities = (ClientEntityManager*)m_systemManager->GetEntityManager();
	C_Position* position = nullptr;
	C_Movable* movable = nullptr;
	S_Movement* movement_s = nullptr;
	S_State* state_s = nullptr;
	//C_Health* health = nullptr;
	//C_Name* name = nullptr;
	//m_client->Lock();
	//std::cout << "Network apply. Take lock at: " << clock.getCurrentTime() << std::endl;
	if (movable = entities->GetComponent<C_Movable>(l_entity, Component::Movable)) {
		if (position = entities->GetComponent<C_Position>(l_entity, Component::Position)) {
			//std::cout << "Position old: " << position->GetPosition().first << " : " << position->GetPosition().second << std::endl;
			sf::Vector2f newPosition = sf::Vector2f((position->GetPosition().first + l_snapshot.m_position.first) / 2.f, (position->GetPosition().second + l_snapshot.m_position.second) / 2.f);
			position->SetPosition(newPosition);
			//std::cout << "Position new: " << position->GetPosition().first << " : " << position->GetPosition().second << std::endl;
			position->SetElevation(l_snapshot.m_elevation);
		}
	}
	if (l_applyPhysics){
		if (movable = entities->GetComponent<C_Movable>(l_entity, Component::Movable)){
			movable->SetVelocity(l_snapshot.m_velocity);
			movable->SetAcceleration(l_snapshot.m_acceleration);
		}
	}
	if (movable = entities->GetComponent<C_Movable>(l_entity, Component::Movable)) {
		if (movement_s = m_systemManager->GetSystem<S_Movement>(System::Movement)) {
			movement_s->SetDirection(l_entity, (Direction)l_snapshot.m_direction);
		}
		if (state_s = m_systemManager->GetSystem<S_State>(System::State)){
			state_s->ChangeState(l_entity, (EntityState)l_snapshot.m_state, true);
		}
	}
	/*if (health = entities->GetComponent<C_Health>(l_entity, Component::Health)){
		health->SetHealth(l_snapshot.m_health);
	}
	if (name = entities->GetComponent<C_Name>(l_entity, Component::Name)){
		name->SetName(l_snapshot.m_name);
	}*/
	//m_client->Unlock();
	//LEAVE
	//std::cout << "Network apply. Leave at: " << clock.getCurrentTime() << std::endl;
}

void S_Network::SendPlayerOutgoing(sf::Time l_dT){
	INT32 p_x = 0, p_y = 0;
	INT32 p_a = 0;
	
	
	for (auto &itr : m_outgoing){
		if (itr.first == EntityMessage::Move){
			INT32 x = 0, y = 0;
			for (auto &message : itr.second){
				if (message.m_int == (INT32)Direction::Up){ 
					//std::cout << "Up" << std::endl;
					--y; 
				}
				else if (message.m_int == (INT32)Direction::Down){ 
					//std::cout << "Down" << std::endl;
					++y; 
				}
				else if (message.m_int == (INT32)Direction::Left){ 
					//std::cout << "Left" << std::endl;
					--x; 
				}
				else if (message.m_int == (INT32)Direction::Right){
					//std::cout << "Right" << std::endl;
					++x; 
				}
			}
			if (!x && !y){ continue; }
			p_x = x; p_y = y;
		} else if (itr.first == EntityMessage::Bullet_add)
		{ 
			for (auto& message : itr.second) {
				Packet bulletSpawnPacket;
				StampPacket(PacketType::BulletSpawn, bulletSpawnPacket);
				bulletSpawnPacket << INT32(EntityMessage::Attack) << message.m_sender << 
					message.m_receiver << message.m_4f.d_x << message.m_4f.d_y
					<< (float)(message.m_4f.p_x + message.m_4f.d_x * l_dT) << (float)(message.m_4f.p_y + message.m_4f.d_y * l_dT) << INT32(Network::PlayerUpdateDelim);
				std::cout << "Receive attak sender: " << message.m_sender << " receiver: "
					<< message.m_receiver << " m_x: " << message.m_4f.p_x + message.m_4f.d_x * l_dT << " p_y: "
					<< message.m_4f.p_y + message.m_4f.d_y * l_dT << " speed : " << message.m_4f.d_x << " : " << message.m_4f.d_y << std::endl;
				m_client->Send(bulletSpawnPacket);
			}
		}
	}
	Packet packet;
	StampPacket(PacketType::PlayerUpdate, packet);
	packet << INT32(EntityMessage::Move) << p_x << p_y << INT32(Network::PlayerUpdateDelim);
	
	m_client->Send(packet);
	m_outgoing.clear();
}

void S_Network::ClearSnapshots(){ m_entitySnapshots.clear(); }

void S_Network::PerformInterpolation(){
	if (m_entitySnapshots.empty()){ return; }
	ClientEntityManager* entities = (ClientEntityManager*)m_systemManager->GetEntityManager();
	sf::Time t = m_client->GetTime();
	auto itr = ++m_entitySnapshots.begin();
	while (itr != m_entitySnapshots.end()){
		if (m_entitySnapshots.begin()->first <= t - NET_RENDER_DELAY
			&& itr->first >= t - NET_RENDER_DELAY)
		{
			auto Snapshot1 = m_entitySnapshots.begin();
			auto Snapshot2 = itr;
			bool SortDrawables = false;
			for (auto snap = Snapshot1->second.m_snapshots.begin();
				snap != Snapshot1->second.m_snapshots.end();)
			{
				if (!entities->HasEntity(snap->first)){
					if (entities->AddEntity(snap->second.m_type, snap->first) == (int)Network::NullID){
						std::cout << "Failed adding entity type: " << snap->second.m_type << std::endl;
						++snap;
						continue;
					}
					ApplyEntitySnapshot(snap->first, snap->second, true);
					++snap;
					continue;
				}
				auto snap2 = Snapshot2->second.m_snapshots.find(snap->first);
				if (snap2 == Snapshot2->second.m_snapshots.end()){
					// Entity that exists in first snapshot wasn't found in second.
					// Remove it, as it possibly de-spawned.
					//m_client->Lock();
					//std::cout << "Network interpol. Take lock at: " << clock.getCurrentTime() << std::endl;
					entities->RemoveEntity(snap->first);
					snap = Snapshot1->second.m_snapshots.erase(snap);
					//LEAVE
					//m_client->Unlock();
					//std::cout << "Network interpol. Leave at: " << clock.getCurrentTime() << std::endl;
					continue;
				}

				EntitySnapshot i_snapshot;
				InterpolateSnapshot(snap->second, Snapshot1->first,
					snap2->second, Snapshot2->first,
					i_snapshot, t - NET_RENDER_DELAY);
				ApplyEntitySnapshot(snap->first, i_snapshot, true);
				if (!CompareSnapshots(snap->second, snap2->second, true, false, false)){
					SortDrawables = true;
				}
				++snap;
			}
			if (SortDrawables){ m_systemManager->GetSystem<S_Renderer>(System::Renderer)->SortDrawables(); }
			return;
		}
		m_entitySnapshots.erase(m_entitySnapshots.begin());
		itr = ++m_entitySnapshots.begin();
	}
}