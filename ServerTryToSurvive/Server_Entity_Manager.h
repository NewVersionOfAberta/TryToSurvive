#pragma once

#include "Entity_Manager.h"
#include "C_Position.h"
#include "C_State.h"
#include "C_Movable.h"
#include "C_Collidable.h"
#include "C_Client.h"
#include "C_Attacker.h"
#include "Packet.h"
#include "EntitySnapshot.h"

class SystemManager;

class ServerEntityManager : public EntityManager{
public:
	ServerEntityManager(SystemManager* l_sysMgr);
	~ServerEntityManager();

	void DumpEntityInfo(Packet& l_packet);
private:
};