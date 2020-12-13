#include "EntitySnapshot.h"

Packet& operator <<(Packet& l_packet, const EntitySnapshot& l_snapshot){
	return l_packet << l_snapshot.m_type << l_snapshot.m_name << l_snapshot.m_position.first << l_snapshot.m_position.second << l_snapshot.m_elevation
		<< l_snapshot.m_velocity.first << l_snapshot.m_velocity.second << l_snapshot.m_acceleration.first
		<< l_snapshot.m_acceleration.second << l_snapshot.m_direction << l_snapshot.m_state << l_snapshot.m_health;
}

Packet& operator >>(Packet& l_packet, EntitySnapshot& l_snapshot){
	return l_packet >> l_snapshot.m_type >> l_snapshot.m_name >> l_snapshot.m_position.first >> l_snapshot.m_position.second >> l_snapshot.m_elevation
		>> l_snapshot.m_velocity.first >> l_snapshot.m_velocity.second >> l_snapshot.m_acceleration.first
		>> l_snapshot.m_acceleration.second >> l_snapshot.m_direction >> l_snapshot.m_state >> l_snapshot.m_health;
}