#include "S_Bullets.h"
#include "System_Manager.h"
#include "C_Attacker.h"


#define POS_OFFSET 20
#define BULLET_ELEVATION 1
#define BULLET_WIDTH 20.0
#define BULLET_HEIGHT 20.0
#define SPEED 400


S_Bullets::S_Bullets(SystemManager* l_systemMgr)
	: S_Base(System::Bullets, l_systemMgr), m_currId(5), m_bullets()//, m_playerId(0)
{
	m_bullets.clear();
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	req.TurnOnBit((unsigned int)Component::Attacker);
	//!
	//req.TurnOnBit((unsigned int)Component::Shape);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Attack, this);
	//m_systemManager->GetMessageHandler()->Subscribe(EntityMessage::Bullet_add, this);
}

S_Bullets::~S_Bullets()
{
	m_bullets.clear();
}

void S_Bullets::Update(float l_dT)
{
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	C_Position* position;
	for (auto& bullet : m_bullets) {
		position = eMgr->GetComponent<C_Position>(bullet.m_id, Component::Position);
		if (!position) {
			auto delBullet = std::find_if(m_bullets.begin(), m_bullets.end(),
				[bullet](Bullet b) { return b.m_id == bullet.m_id; });
			if (delBullet != m_bullets.end()) {
				m_bullets.erase(delBullet);
			}
			continue;
		}
		sf::Vector2f newPosition(
			position->GetPosition().first + l_dT * bullet.m_speedX, position->GetPosition().second + l_dT * bullet.m_speedY);
		position->SetPosition(newPosition);
		std::cout << "Update bullet pos : " << newPosition.first << " : " << newPosition.second << std::endl;
	}

}

void S_Bullets::HandleEvent(const EntityId& l_entity, const EntityEvent& l_event)
{
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	switch (l_event) {
	case EntityEvent::Colliding_X:
	case EntityEvent::Colliding_Y:
	{
		std::cout << "Remove bullet: " << l_entity << std::endl;
		//eMgr->RemoveEntity(l_entity);

		auto bullet = std::find_if(m_bullets.begin(), m_bullets.end(),
			[l_entity](Bullet b) { return b.m_id == l_entity; });
		if (bullet != m_bullets.end()) {
			m_bullets.erase(bullet);
			eMgr->RemoveEntity(l_entity);
		}
		break;
	}
	}
}

void S_Bullets::Notify(const Message& l_message)
{
	EntityMessage m = (EntityMessage)l_message.m_type;

	switch (m) {
	case EntityMessage::Attack:
//	case EntityMessage::Bullet_add:
	{
		//C_Position* position = eMgr->GetComponent<C_Position>(l_message.m_receiver, Component::Position);
		EntityManager* eMgr = m_systemManager->GetEntityManager();
		sf::Vector2f pos(l_message.m_4f.p_x, l_message.m_4f.p_y);
		sf::Vector2f speed(l_message.m_4f.d_x, l_message.m_4f.d_y);
		CreateBullet(speed, pos, eMgr, l_message.m_sender);
	}
	}
}

void S_Bullets::CreateBullet(sf::Vector2f speed, sf::Vector2f pos, EntityManager* eMgr, int id)
{
	std::cout << "Create bullet: " << id << "on " << pos.first << ", second " << pos.second << " speed :" << speed.first << " : "
		<< speed.second << std::endl;
	
	Bullet bullet(id, speed.first, speed.second);
	m_bullets.push_back(bullet);

	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	req.TurnOnBit((unsigned int)Component::Attacker);
	//!
	//req.TurnOnBit((unsigned int)Component::Shape);

	eMgr->AddEntity(req, id);
	req.Clear();

	C_Position* positionBullet = eMgr->GetComponent<C_Position>(id, Component::Position);
	if (!positionBullet) { return; }
	positionBullet->SetPosition(sf::Vector2f(pos.first, pos.second));
	positionBullet->SetElevation(BULLET_ELEVATION);

	C_Collidable* collisionBullet = eMgr->GetComponent<C_Collidable>(id, Component::Collidable);
	if (!collisionBullet) { return; }
	std::cout << "Create collidable: " << id << std::endl;
	
	collisionBullet->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	collisionBullet->SetOrigin(Origin::Top_Left);

	C_Attacker* attacker = eMgr->GetComponent<C_Attacker>(id, Component::Attacker);
	attacker->SetAreaPosition(sf::Vector2f(pos.first, pos.second));
	attacker->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	/*C_Shape* shapeBullet = eMgr->GetComponent<C_Shape>(m_currId, Component::Shape);
	if (!shapeBullet) { return; }
	shapeBullet->SetSize(sf::Vector2u(BULLET_WIDTH, BULLET_HEIGHT));*/
	//++m_currId;
}
