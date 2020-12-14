#pragma once
#include "S_Base.h"
#include "C_Position.h"
#include "C_Collidable.h"
//#include "C_Shape.h"
#include "Usings.h"

struct Bullet {
	Bullet(int l_id, float l_speedX, float l_speedY) : m_speedX(l_speedX), m_speedY(l_speedY), m_id(l_id) {}
	INT32 m_id;
	float m_speedX;
	float m_speedY;
};

using Bullets = std::vector<Bullet>;

class S_Bullets : public S_Base
{
public:
	S_Bullets(SystemManager* l_systemMgr);
	~S_Bullets();

	void Update(float l_dT);
	void HandleEvent(const EntityId& l_entity, const EntityEvent& l_event);
	void Notify(const Message& l_message);
	//void SetPlayerID(int l_playerId) { m_playerId = l_playerId; }

private:
	void CreateBullet(sf::Vector2f mousePos, sf::Vector2f entityPos, EntityManager* eMgr, int id);
	Bullets m_bullets;
	int m_currId;
	//int m_playerId;
};

