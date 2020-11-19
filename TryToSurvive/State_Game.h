#pragma once
#include "BaseState.h"
#include <unordered_map>
#include "EventManager.h"
#include "Usings.h"

class State_Game : public BaseState {
public:
	State_Game(StateManager* l_stateManager);
	~State_Game();

	void OnCreate();
	void OnDestroy();

	void Activate();
	void Deactivate();

	void Update(const sf::Time& l_time);
	void Draw();

	void MainMenu(EventDetails* l_details);
	void PlayerMove(EventDetails* l_details);
	//void PlayerAttack(EventDetails* l_details);

	//void HandlePacket(const PacketID& l_id, sf::Packet& l_packet, Client* l_client);

	// Debug:
	//void ToggleOverlay(EventDetails* l_details);
private:
	void UpdateCamera();
	//Map* m_gameMap;
	int m_player;
	//Client* m_client;
};

