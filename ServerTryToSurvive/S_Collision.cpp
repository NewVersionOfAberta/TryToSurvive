#include "S_Collision.h"
#include "System_Manager.h"
#include "Map.h"

S_Collision::S_Collision(SystemManager* l_systemMgr)
	: S_Base(System::Collision, l_systemMgr)
{
	Bitmask req;
	req.TurnOnBit((unsigned int)Component::Position);
	req.TurnOnBit((unsigned int)Component::Collidable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_gameMap = nullptr;
}

S_Collision::~S_Collision(){}

void S_Collision::SetMap(Map* l_map){ m_gameMap = l_map; }

void S_Collision::Update(float l_dT){
	if (!m_gameMap){ return; }
	EntityManager* entities = m_systemManager->GetEntityManager();
	for (auto &entity : m_entities){
		//std::cout << "Entity id: " << entity << std::endl;
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Collidable* collidable = entities->GetComponent<C_Collidable>(entity, Component::Collidable);

		collidable->SetPosition(position->GetPosition());
		collidable->ResetCollisionFlags();
		CheckOutOfBounds(position, collidable, entity);
		MapCollisions(entity, position, collidable);
		//std::cout << "Position: " << position->GetPosition().first << position->GetPosition().second << std::endl;
	}
	EntityCollisions();
}

void S_Collision::EntityCollisions(){
	EntityManager* entities = m_systemManager->GetEntityManager();
	sf::FloatRect Intersection;
	for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr)
	{
		for (auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr2){
			
		
			C_Attacker* attacker1 = entities->GetComponent<C_Attacker>(*itr, Component::Attacker);
			C_Attacker* attacker2 = entities->GetComponent<C_Attacker>(*itr2, Component::Attacker);
			if (attacker1 && attacker2){ 
				m_systemManager->AddEvent(*itr, (EventID)EntityEvent::Colliding_X);
				m_systemManager->AddEvent(*itr2, (EventID)EntityEvent::Colliding_X);
				continue;
			}
			Message msg((MessageType)EntityMessage::Being_Attacked);
			if (attacker1 && !attacker2){
				C_Collidable* collidable2 = entities->GetComponent<C_Collidable>(*itr2, Component::Collidable);
				if (attacker1->GetAreaOfAttack().intersects(collidable2->GetCollidable(), Intersection)){
					// Attacker-on-entity collision!
					msg.m_receiver = *itr2;
					msg.m_sender = *itr;
					m_systemManager->GetMessageHandler()->Dispatch(msg);
				}
			}
			if (attacker2 && !attacker1){
				C_Collidable* collidable1 = entities->GetComponent<C_Collidable>(*itr, Component::Collidable);
				if (attacker2->GetAreaOfAttack().intersects(collidable1->GetCollidable(), Intersection)){
					// Attacker-on-entity collision!
					msg.m_receiver = *itr;
					msg.m_sender = *itr2;
					m_systemManager->GetMessageHandler()->Dispatch(msg);
				}
			}
		}
	}
}

void S_Collision::CheckOutOfBounds(C_Position* l_pos, C_Collidable* l_col, int l_entity){
	unsigned int TileSize = m_gameMap->GetTileSize();

	if (l_pos->GetPosition().first < 0){
		l_pos->SetPosition(0.0f, l_pos->GetPosition().second);
		l_col->SetPosition(l_pos->GetPosition());
		m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_X);
	} else if (l_pos->GetPosition().first > m_gameMap->GetMapSize().first * TileSize){
		l_pos->SetPosition(m_gameMap->GetMapSize().first * TileSize, l_pos->GetPosition().second);
		l_col->SetPosition(l_pos->GetPosition());
		m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_X);
	}

	if (l_pos->GetPosition().second < 0){
		l_pos->SetPosition(l_pos->GetPosition().first, 0.0f);
		l_col->SetPosition(l_pos->GetPosition());
		m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
	} else if (l_pos->GetPosition().second > m_gameMap->GetMapSize().second * TileSize){
		l_pos->SetPosition(l_pos->GetPosition().first, m_gameMap->GetMapSize().second * TileSize);
		l_col->SetPosition(l_pos->GetPosition());
		m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
	}
}

void S_Collision::MapCollisions(const EntityId& l_entity, C_Position* l_pos, C_Collidable* l_col){
	unsigned int TileSize = m_gameMap->GetTileSize();
	Collisions c;

	sf::FloatRect EntityAABB = l_col->GetCollidable();
	int FromX = floor(EntityAABB.left / TileSize);
	int ToX = floor((EntityAABB.left + EntityAABB.width) / TileSize);
	int FromY = floor(EntityAABB.top / TileSize);
	int ToY = floor((EntityAABB.top + EntityAABB.height) / TileSize);


	for (int x = FromX; x <= ToX; ++x) {
		for (int y = FromY; y <= ToY; ++y) {
			for (int l = 0; l < Sheet::Num_Layers; ++l) {
				Tile* t = m_gameMap->GetTile(x, y, l);
				if (!t) { continue; }
				if (!t->m_properties->m_solid) { continue; }
				sf::FloatRect TileAABB(sf::Vector2f(x * TileSize, y * TileSize), sf::Vector2f(TileSize, TileSize));
				sf::FloatRect Intersection;
				EntityAABB.intersects(TileAABB, Intersection);
				float S = Intersection.width * Intersection.height;
				if (S > 0.1) {
					c.emplace_back(S, t->m_properties, TileAABB);
				}
				break;
			}
		}
	}

	if (c.empty()) { return; }
	std::sort(c.begin(), c.end(), [](CollisionElement& l_1, CollisionElement& l_2) {
		return l_1.m_area > l_2.m_area;
		});

	for (auto& col : c) {
		EntityAABB = l_col->GetCollidable();
		//if (!col.m_tileBounds.intersects(EntityAABB)) { continue; }

		float xDiff = (EntityAABB.left + (EntityAABB.width / 2)) - (col.m_tileBounds.left + (col.m_tileBounds.width / 2));
		float yDiff = (EntityAABB.top + (EntityAABB.height / 2)) - (col.m_tileBounds.top + (col.m_tileBounds.height / 2));
		float resolve = 0;
		if (std::abs(xDiff) > std::abs(yDiff)) {
			if (xDiff > 0) {
				resolve = (col.m_tileBounds.left + TileSize) - EntityAABB.left;
			}
			else {
				resolve = -((EntityAABB.left + EntityAABB.width) - col.m_tileBounds.left);
			}
			l_pos->MoveBy(resolve, 0);
			l_col->SetPosition(l_pos->GetPosition());

			std::cout << "Collison Detected on x" << std::endl;
			m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_X);
			l_col->CollideOnX();
		}
		else {
			if (yDiff > 0) {
				resolve = (col.m_tileBounds.top + TileSize) - EntityAABB.top;
			}
			else {
				resolve = -((EntityAABB.top + EntityAABB.height) - col.m_tileBounds.top);
			}
			l_pos->MoveBy(0, resolve);
			l_col->SetPosition(l_pos->GetPosition());
			std::cout << "Collison Detected on y" << std::endl;
			m_systemManager->AddEvent(l_entity, (EventID)EntityEvent::Colliding_Y);
			l_col->CollideOnY();
		}
	}
}

void S_Collision::HandleEvent(const EntityId& l_entity,const EntityEvent& l_event){}
void S_Collision::Notify(const Message& l_message){}