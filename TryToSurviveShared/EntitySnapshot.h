#pragma once
#include "Packet.h"

struct EntitySnapshot{
	std::string m_type;
	sf::Vector2f m_position;
	INT32 m_elevation;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
	INT32 m_direction;
	INT32 m_state;
	INT32 m_health;
	std::string m_name;
};

Packet& operator <<(Packet& l_packet, const EntitySnapshot& l_snapshot);
Packet& operator >>(Packet& l_packet, EntitySnapshot& l_snapshot);