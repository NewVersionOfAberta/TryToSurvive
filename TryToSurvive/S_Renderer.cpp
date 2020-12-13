#include "S_Renderer.h"
#include "System_Manager.h"
#include "C_SpriteSheet.h"

S_Renderer::S_Renderer(SystemManager* l_systemMgr)
	:S_Base(System::Renderer, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::SpriteSheet);
	m_requiredComponents.push_back(req);
	req.Clear();
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Shape);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed, this);
}

S_Renderer::~S_Renderer() {}

void S_Renderer::Update(float l_dT) {
	l_dT /= 1000;
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto& entity : m_entities)
	{
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Drawable* drawable = nullptr;
		if (entities->HasComponent(entity, Component::SpriteSheet)) {
			drawable = entities->GetComponent<C_Drawable>(entity, Component::SpriteSheet);
		}
		else if (entities->HasComponent(entity, Component::Shape)) {
			drawable = entities->GetComponent<C_Drawable>(entity, Component::Shape);
		}
		else {
			continue;
		}
		drawable->UpdatePosition(position->GetPosition());
		
	}
}

void S_Renderer::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event)
{
	if (l_event == EntityEvent::Moving_Left || l_event == EntityEvent::Moving_Right ||
		l_event == EntityEvent::Moving_Up || l_event == EntityEvent::Moving_Down ||
		/*l_event == EntityEvent::Elevation_Change ||*/ l_event == EntityEvent::Spawned)
	{
		SortDrawables();
	}
}

void S_Renderer::Notify(const Message& l_message) {
	if (!HasEntity(l_message.m_receiver)) { return; }
	EntityMessage m = (EntityMessage)l_message.m_type;
	switch (m) {
	case EntityMessage::Direction_Changed:
		SetSheetDirection(l_message.m_receiver, (Direction)l_message.m_int);
		break;
	}
}

void S_Renderer::Render(Window* l_wind, unsigned int l_layer)
{
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		if (position->GetElevation() < l_layer) { continue; }
		if (position->GetElevation() > l_layer) { break; }
		C_Drawable* drawable = nullptr;
		Component component;
		if (!entities->HasComponent(entity, Component::SpriteSheet) 
			&& !entities->HasComponent(entity, Component::Shape)) { continue; }
		component = entities->HasComponent(entity, Component::SpriteSheet) ? Component::SpriteSheet : Component::Shape;
		drawable = entities->GetComponent<C_Drawable>(entity, component);
		sf::FloatRect drawableBounds;
		drawableBounds.left = position->GetPosition().first;
		drawableBounds.top = position->GetPosition().second;
		drawableBounds.width = drawable->GetSize().first;
		drawableBounds.height = drawable->GetSize().second;
		sf::FloatRect intersection;
		if (!drawableBounds.intersects(l_wind->GetViewSpace(), intersection)) { continue; }
		drawable->Draw(l_wind);
	}
}

void S_Renderer::SetSheetDirection(const EntityId& l_entity, const Direction& l_dir)
{
	EntityManager* entities = m_systemManager->GetEntityManager();
	if (!entities->HasComponent(l_entity, Component::SpriteSheet)) { return; }
	C_SpriteSheet* sheet = entities->GetComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
	sheet->GetSpriteSheet()->SetDirection(l_dir);
}

void S_Renderer::SortDrawables() {
	EntityManager* e_mgr = m_systemManager->GetEntityManager();
	std::sort(m_entities.begin(), m_entities.end(),
		[e_mgr](unsigned int l_1, unsigned int l_2)
		{
			auto pos1 = e_mgr->GetComponent<C_Position>(l_1, Component::Position);
			auto pos2 = e_mgr->GetComponent<C_Position>(l_2, Component::Position);
			if (pos1->GetElevation() == pos2->GetElevation()) {
				return pos1->GetPosition().second < pos2->GetPosition().second;
			}
			return pos1->GetElevation() < pos2->GetElevation();
		});
}