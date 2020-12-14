#include "S_Movement.h"
#include "System_Manager.h"
#include "Map.h"
#include "Clock.h"

S_Movement::S_Movement(SystemManager* l_systemMgr)
	: S_Base(System::Movement, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Movable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Is_Moving, this);

	m_gameMap = nullptr;
}

S_Movement::~S_Movement() {}

void S_Movement::Update(float l_dT) {
	
	if (!m_gameMap) { return; }
	//l_dT = Clock::millsAsSeconds(l_dT);
	//l_dT /= 1000;
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto& entity : m_entities) {
		//std::cout << "Entity id: " << entity << std::endl;
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Movable* movable = entities->GetComponent<C_Movable>(entity, Component::Movable);
		//temp
		sf::Vector2f velocity = movable->GetVelocity();
		//std::cout << "Velocity before step: " << velocity.first << " : " << velocity.second << std::endl;
		MovementStep(l_dT, movable, position);
		velocity = movable->GetVelocity();
	
		position->MoveBy(velocity.first * l_dT, velocity.second * l_dT);
		//std::cout << "Position: " << position->GetPosition().first << " : " << position->GetPosition().second << std::endl;
	}
}

void S_Movement::HandleEvent(const EntityId& l_entity,
	const EntityEvent& l_event)
{

	switch (l_event) {
	case EntityEvent::Colliding_X: StopEntity(l_entity, Axis::x); break;
	case EntityEvent::Colliding_Y: StopEntity(l_entity, Axis::y); break;
	case EntityEvent::Moving_Left: SetDirection(l_entity, Direction::Left); break;
	case EntityEvent::Moving_Right: SetDirection(l_entity, Direction::Right); break;
	case EntityEvent::Moving_Up:
	{
		C_Movable* mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
		if (mov->GetVelocity().first == 0) { SetDirection(l_entity, Direction::Up); }
	}
	break;
	case EntityEvent::Moving_Down:
	{
		C_Movable* mov = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
		if (mov->GetVelocity().first == 0) { SetDirection(l_entity, Direction::Down); }
	}
	break;
	}
}

void S_Movement::Notify(const Message& l_message) {
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	EntityMessage m = (EntityMessage)l_message.m_type;
	switch (m) {
	case EntityMessage::Is_Moving:
	{
		if (!HasEntity(l_message.m_receiver)) { return; }
		C_Movable* movable = eMgr->GetComponent<C_Movable>
			(l_message.m_receiver, Component::Movable);
		if (movable->GetVelocity() != sf::Vector2f(0.0f, 0.0f)) { return; }
		m_systemManager->AddEvent(l_message.m_receiver, (EventID)EntityEvent::Became_Idle);
	}
	break;
	}
}

const float S_Movement::GetTileFriction(unsigned int l_elevation,
	unsigned int l_x, unsigned int l_y)
{
	Tile* t = nullptr;
	while (!t && l_elevation >= 0) {
		t = m_gameMap->GetTile(l_x, l_y, l_elevation);
		--l_elevation;
	}

	return(t ? t->m_properties->m_friction :
		m_gameMap->GetDefaultTile()->m_friction);
}

void S_Movement::MovementStep(float l_dT, C_Movable* l_movable, C_Position* l_position) {
	float f_coefficient = GetTileFriction(l_position->GetElevation(),
		floor(l_position->GetPosition().first / Sheet::Tile_Size),
		floor(l_position->GetPosition().second / Sheet::Tile_Size));
	//l_dT /= 1000;
	sf::Vector2f friction(l_movable->GetSpeed().first * f_coefficient * l_dT,
		l_movable->GetSpeed().second * f_coefficient * l_dT);
	/*friction.first *= l_dT;
	friction.second *= l_dT;*/
	sf::Vector2f acceleration = l_movable->GetAcceleration();
	/*acceleration.first *= l_dT;
	acceleration.second *= l_dT;*/
	l_movable->AddVelocity(sf::Vector2f(acceleration.first* l_dT, acceleration.second * l_dT));
	l_movable->SetAcceleration(sf::Vector2f(0.0f, 0.0f));
	l_movable->ApplyFriction(friction);

	float magnitude = sqrt(
		(l_movable->GetVelocity().first * l_movable->GetVelocity().first) +
		(l_movable->GetVelocity().second * l_movable->GetVelocity().second));

	if (magnitude <= l_movable->GetMaxVelocity()) { return; }
	float max_V = l_movable->GetMaxVelocity();
	l_movable->SetVelocity(sf::Vector2f(
		(l_movable->GetVelocity().first / magnitude) * max_V,
		(l_movable->GetVelocity().second / magnitude) * max_V));
	//std::cout << "Velocity set old: " << l_movable->GetVelocity().first << " : " << l_movable->GetVelocity().second << std::endl;

}

void S_Movement::SetMap(Map* l_gameMap) { m_gameMap = l_gameMap; }

void S_Movement::StopEntity(const EntityId& l_entity,
	const Axis& l_axis)
{
	C_Movable* movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
	if (l_axis == Axis::x) {
		movable->SetVelocity(sf::Vector2f(0.f, movable->GetVelocity().second));
	}
	else if (l_axis == Axis::y) {
		movable->SetVelocity(sf::Vector2f(movable->GetVelocity().first, 0.f));
	}
}

void S_Movement::SetDirection(const EntityId& l_entity, const Direction& l_dir)
{
	C_Movable* movable = m_systemManager->GetEntityManager()->GetComponent<C_Movable>(l_entity, Component::Movable);
	movable->SetDirection(l_dir);

	Message msg((MessageType)EntityMessage::Direction_Changed);
	msg.m_receiver = l_entity;
	msg.m_int = (int)l_dir;
	m_systemManager->GetMessageHandler()->Dispatch(msg);
}