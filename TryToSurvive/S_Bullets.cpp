#include "S_Bullets.h"
#include "System_Manager.h"

#define POS_OFFSET 20
#define BULLET_ELEVATION 1
#define BULLET_WIDTH 20.0
#define BULLET_HEIGHT 20.0
#define SPEED 500

S_Bullets::S_Bullets(SystemManager* l_systemMgr)
	: S_Base(System::Bullets, l_systemMgr), m_currId(5), m_bullets()//, m_playerId(0)
{
	m_bullets.clear();
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	req.TurnOnBit((unsigned int)Component::Attacker);
	//!
	req.TurnOnBit((unsigned int)Component::Shape);
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
	C_Attacker* attack;
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
		attack = eMgr->GetComponent<C_Attacker>(bullet.m_id, Component::Attacker);
		if (!attack) { return; }
		attack->SetAreaPosition(newPosition);
		//std::cout << "Update bullet pos : " << newPosition.first << " : " << newPosition.second << std::endl;
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
		if (bullet != m_bullets.end()) {
			m_bullets.erase(bullet);
			std::cout << "Remove bullet: " << l_entity << std::endl;
			eMgr->RemoveEntity(l_entity);
		}
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
	//case EntityMessage::Bullet_add:
	{
		C_Position* position = eMgr->GetComponent<C_Position>(l_message.m_receiver, Component::Position);
		sf::Vector2f entityPos = position->GetPosition();
		sf::Vector2f mousePos(l_message.m_2f.m_x, l_message.m_2f.m_y);
		CreateBullet(mousePos, entityPos, eMgr, l_message.m_sender, l_message.m_receiver);
	}
	}
}

void S_Bullets::AddBullet(sf::Vector2f pos, sf::Vector2f speed, int id, int owner)
{
	EntityManager* eMgr = m_systemManager->GetEntityManager();
	Bullet bullet(id, speed.first, speed.second);
	m_bullets.push_back(bullet);

	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	//!
	req.TurnOnBit((unsigned int)Component::Shape);
	req.TurnOnBit((unsigned int)Component::Attacker);

	eMgr->AddEntity(req, id);
	req.Clear();

	C_Position* positionBullet = eMgr->GetComponent<C_Position>(id, Component::Position);
	if (!positionBullet) { return; }
	positionBullet->SetPosition(sf::Vector2f(pos.first, pos.second));
	positionBullet->SetElevation(BULLET_ELEVATION);

	C_Collidable* collisionBullet = eMgr->GetComponent<C_Collidable>(id, Component::Collidable);
	if (!collisionBullet) { return; }
	collisionBullet->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	collisionBullet->SetOrigin(Origin::Top_Left);

	C_Attacker* attacker = eMgr->GetComponent<C_Attacker>(id, Component::Attacker);
	if (!attacker) { return; }
	attacker->SetAreaPosition(sf::Vector2f(pos.first, pos.second));
	attacker->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	attacker->SetOwner(owner);
}

void S_Bullets::CreateBullet(sf::Vector2f mousePos, sf::Vector2f entityPos, EntityManager* eMgr, int id, int eid)
{
	//std::cout << "Create bullet: " << id << std::endl;
	float deltaX = mousePos.first - (entityPos.first + POS_OFFSET);
	float deltaY = mousePos.second - (entityPos.second + POS_OFFSET);
	float maxSpeed = sqrt(deltaX * deltaX + deltaY * deltaY);
	float speedX = deltaX / maxSpeed * SPEED;
	float speedY = deltaY / maxSpeed * SPEED;
	//std::cout << "Speed : " << speedX << " : " << speedY << std::endl;
	Bullet bullet(id, speedX, speedY);
	m_bullets.push_back(bullet);

	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	//!
	req.TurnOnBit((unsigned int)Component::Shape);
	req.TurnOnBit((unsigned int)Component::Attacker);

	eMgr->AddEntity(req, id);
	req.Clear();

	C_Position* positionBullet = eMgr->GetComponent<C_Position>(id, Component::Position);
	if (!positionBullet) { return; }
	positionBullet->SetPosition(sf::Vector2f(entityPos.first + POS_OFFSET, entityPos.second + POS_OFFSET));
	positionBullet->SetElevation(BULLET_ELEVATION);

	C_Collidable* collisionBullet = eMgr->GetComponent<C_Collidable>(id, Component::Collidable);
	if (!collisionBullet) { return; }
	collisionBullet->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	collisionBullet->SetOrigin(Origin::Top_Left);

	C_Attacker* attacker = eMgr->GetComponent<C_Attacker>(id, Component::Attacker);
	if (!attacker) { return; }
	attacker->SetAreaPosition(sf::Vector2f(entityPos.first + POS_OFFSET, entityPos.second + POS_OFFSET));
	attacker->SetSize(sf::Vector2f(BULLET_WIDTH, BULLET_HEIGHT));
	attacker->SetOwner(eid);
	Message msg((MessageType)EntityMessage::Bullet_add);
	msg.m_sender = id;
	msg.m_receiver = eid;
	msg.m_4f.d_x = speedX;
	msg.m_4f.d_y = speedY;
	msg.m_4f.p_x = entityPos.first + POS_OFFSET;
	msg.m_4f.p_y = entityPos.second + POS_OFFSET;

	m_systemManager->GetMessageHandler()->Dispatch(msg);

	/*C_Shape* shapeBullet = eMgr->GetComponent<C_Shape>(m_currId, Component::Shape);
	if (!shapeBullet) { return; }
	shapeBullet->SetSize(sf::Vector2u(BULLET_WIDTH, BULLET_HEIGHT));*/
	//++m_currId;
}
