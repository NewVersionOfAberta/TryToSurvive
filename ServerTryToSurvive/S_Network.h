#pragma once

#include <Windows.h>
#include "S_Base.h"
#include "NetworkDefinitions.h"
#include "Server.h"
#include "Server_Entity_Manager.h"
#include "C_Client.h"
#include "S_State.h"
#include "S_Movement.h"


#pragma comment (lib, "Ws2_32.lib")

struct PlayerInput{
	int m_movedX;
	int m_movedY;
	float m_x;
	float m_y;
	PlayerInput() :m_movedX(0), m_movedY(0), m_x(0.0), m_y(0.0){}
};

using PlayerInputContainer = std::unordered_map<EntityId, PlayerInput>;

class S_Network : public S_Base{
public:
	S_Network(SystemManager* l_systemMgr);
	~S_Network();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);

	bool RegisterClientID(const EntityId& l_entity, const ClientID& l_client);
	void RegisterServer(Server* l_server);
	ClientID GetClientID(const EntityId& l_entity);
	EntityId GetEntityID(const ClientID& l_client);

	void CreateSnapshot(Packet& l_packet);
	void AddBullet(Packet& l_packet, const ClientID& l_cid);
	void UpdatePlayer(Packet& l_packet, const ClientID& l_cid);
private:
	PlayerInputContainer m_playerInput;
	Server* m_server;
};