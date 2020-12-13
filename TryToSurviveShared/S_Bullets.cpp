#include "S_Bullets.h"
#include "System_Manager.h"

#define POS_OFFSET 20
#define BULLET_ELEVATION 1
#define BULLET_WIDTH 20.0
#define BULLET_HEIGHT 20.0
#define SPEED 500

S_Bullets::S_Bullets(SystemManager* l_systemMgr)
	: S_Base(System::Bullets, l_systemMgr), m_currId(5), m_bullets(), m_playerId(0)
{
	m_bullets.clear();
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	//!
	req.TurnOnBit((unsigned int)Component::Shape);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);

}

S_Bullets::~S_Bullets()
{
	m_bullets.clear();
}

void S_Bullets::Update(float l_dT)
{
	l_dT /= 1000;
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	C_Position* position;
	for (auto& bullet : m_bullets) {
		position = eMgr->GetComponent<C_Position>(bullet.m_id, Component::Position);
		sf::Vector2f newPosition(
			position->GetPosition().first + l_dT * bullet.m_speedX, position->GetPosition().second + l_dT * bullet.m_speedY);
		position->SetPosition(newPosition);
	}

}

void S_Bullets::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event)
{
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	switch (l_event) {
	case EntityEvent::Colliding_X:
	case EntityEvent::Colliding_Y:
	{
		eMgr->RemoveEntity(l_entity);
		auto bullet = std::find_if(m_bullets.begin(), m_bullets.end(),
			[l_entity](Bullet b) { return b.m_id == l_entity; });
		m_bullets.erase(bullet);
		break;
	}
	}
}

void S_Bullets::Notify(const Message& l_message)
{
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	EntityMessage m = (EntityMessage)l_message.m_type;

	switch (m) {
	case EntityMessage::Attack:
	{
		C_Position* position = eMgr->GetComponent<C_Position>(m_playerId, Component::Position);
		sf::Vector2f entityPos = position->GetPosition();
		sf::Vector2f mousePos(l_message.m_2f.m_x, l_message.m_2f.m_y);
		CreateBullet(mousePos, entityPos, eMgr);
	}
	}
}

void S_Bullets::CreateBullet(sf::Vector2f mousePos, sf::Vector2f entityPos, EntityManager* eMgr)
{
	float deltaX = mousePos.first - (entityPos.first + POS_OFFSET);
	float deltaY = mousePos.second - (entityPos.second + POS_OFFSET);
	float maxSpeed = sqrt(deltaX * deltaX + deltaY * deltaY);
	float speedX = deltaX / maxSpeed * SPEED;
	float speedY = deltaY / maxSpeed * SPEED;
	Bullet bullet(m_currId, speedX, speedY);
	m_bullets.push_back(bullet);

	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	//!
	req.TurnOnBit((unsigned int)Component::Shape);

	m_currId = eMgr->AddEntity(req, m_currId);
	req.Clear();

	C_Position* positionBullet = eMgr->GetComponent<C_Position>(m_currId, Component::Position);
	if (!positionBullet) { return; }
	positionBullet->SetPosition(sf::Vector2f(entityPos.first + POS_OFFSET, entityPos.second + POS_OFFSET));
	positionBullet->SetElevation(BULLET_ELEVATION);

	C_Collidable* collisionBullet = eMgr->GetComponent<C_Collidable>(m_currId, Component::Collidable);
	if (!collisionBullet) { return; }
	collisionBullet->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	collisionBullet->SetOrigin(Origin::Top_Left);

	/*C_Shape* shapeBullet = eMgr->GetComponent<C_Shape>(m_currId, Component::Shape);
	if (!shapeBullet) { return; }
	shapeBullet->SetSize(sf::Vector2u(BULLET_WIDTH, BULLET_HEIGHT));*/
	++m_currId;
}
