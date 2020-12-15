#include "S_CharacterUI.h"

#define OFFSET 10

S_CharacterUI::S_CharacterUI(SystemManager* l_systemMgr) : S_Base(System::Character_UI, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Health);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_heartBarSize = sf::Vector2u(65, 10);
}

S_CharacterUI::~S_CharacterUI()
{
}

void S_CharacterUI::Update(float l_dT)
{
}

void S_CharacterUI::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event)
{
}

void S_CharacterUI::Notify(const Message& l_message)
{
}

void S_CharacterUI::Render(Window* l_wind)
{
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Health* health = entities->GetComponent<C_Health>(entity, Component::Health);
		C_Position* pos = entities->GetComponent<C_Position>(entity, Component::Position);
		if (health && pos) {
			sf::Vector2f heartBarPos(pos->GetPosition().first, pos->GetPosition().second - OFFSET);
			l_wind->DrawRect(heartBarPos, m_heartBarSize, HEART_BAR_COLOR);
			int width = m_heartBarSize.first / health->GetMaxHealth();
			heartBarPos.second += 1;
			for (int i = 0; i < health->GetHealth(); i++) {
				l_wind->DrawRect(heartBarPos, sf::Vector2u(width, m_heartBarSize.second - 1), HEALTH_COLOR);
				heartBarPos.first += width;
			}
		}
	}
}
