#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Packet.h>
using PacketID = INT32;
enum class PacketType{
	Disconnect = -1, Connect, Heartbeat, Snapshot, PlayerUpdate, Message, Hurt, BulletSpawn, OutOfBounds
};

void StampPacket(const PacketType& l_type, Packet& l_packet);