#include "Server_Entity_Manager.h"
#include "System_Manager.h"

ServerEntityManager::ServerEntityManager(SystemManager* l_sysMgr): EntityManager(l_sysMgr){
	AddComponentType<C_Position>(Component::Position);
	AddComponentType<C_State>(Component::State);
	AddComponentType<C_Movable>(Component::Movable);
	AddComponentType<C_Collidable>(Component::Collidable);
	AddComponentType<C_Client>(Component::Client);
	//AddComponentType<C_Health>(Component::Health);
	//AddComponentType<C_Name>(Component::Name);
	AddComponentType<C_Attacker>(Component::Attacker);
}

ServerEntityManager::~ServerEntityManager(){}

void ServerEntityManager::DumpEntityInfo(Packet& l_packet){
	for (auto &entity : m_entities){
		l_packet << INT32(entity.first);
		EntitySnapshot snapshot;
		snapshot.m_type = entity.second.m_type;
		const auto& mask = entity.second.m_bitmask;
		if (mask.GetBit((unsigned int)Component::Position)){
			C_Position* p = GetComponent<C_Position>(entity.first, Component::Position);
			snapshot.m_position = p->GetPosition();
			snapshot.m_elevation = p->GetElevation();
			std::cout << "Id: " << entity.first << " Position: " << snapshot.m_position.first << " : " << snapshot.m_position.second << std::endl;
		}
		if (mask.GetBit((unsigned int)Component::Movable)){
			C_Movable* m = GetComponent<C_Movable>(entity.first, Component::Movable);
			snapshot.m_velocity = m->GetVelocity();
			snapshot.m_acceleration = m->GetAcceleration();
			snapshot.m_direction = UINT8(m->GetDirection());
		}
		if (mask.GetBit((unsigned int)Component::State)){
			C_State* s = GetComponent<C_State>(entity.first, Component::State);
			snapshot.m_state = UINT8(s->GetState());
		}
		if (mask.GetBit((unsigned int)Component::Health)){
			//C_Health* h = GetComponent<C_Health>(entity.first, Component::Health);
			//snapshot.m_health = h->GetHealth();
		}
		if (mask.GetBit((unsigned int)Component::Name)){
			//C_Name* n = GetComponent<C_Name>(entity.first, Component::Name);
			//snapshot.m_name = n->GetName();
		}
		l_packet << snapshot;
	}
}