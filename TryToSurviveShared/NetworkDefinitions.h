#pragma once

enum class Network{
	HighestTimestamp = 2147483647, ClientTimeout = INT_MAX, ServerPort = 27015,
	NullID = -1, PlayerUpdateDelim = -1
};

using ClientID = int;
using PortNumber = unsigned short;