#pragma once
#include "S_Base.h"
#include "System_Manager.h"
#include "Window.h"
#include "C_Health.h"
#include "C_Position.h"


#define HEART_BAR_COLOR RGB(0, 99, 99)
#define HEALTH_COLOR RGB(166, 0, 0)

class S_CharacterUI : public S_Base
{
public:
	S_CharacterUI(SystemManager* l_systemMgr);
	~S_CharacterUI();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);

	void Render(Window* l_wind);
private:
	sf::Vector2u m_heartBarSize;
};


