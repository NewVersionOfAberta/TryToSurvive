#include "PacketTypes.h"
void StampPacket(const PacketType& l_type, Packet& l_packet){
	l_packet << PacketID(l_type);
}