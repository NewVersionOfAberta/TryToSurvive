#pragma once
using MessageType = unsigned int;
struct TwoFloats { 
	TwoFloats() : m_x(0.0), m_y(0.0){}
	float m_x; 
	float m_y; 
};

struct Message {
	Message(const MessageType& l_type) : m_type(l_type), m_sender(0), m_receiver(0), m_2f() {}

	MessageType m_type;
	int m_sender;
	int m_receiver;

	union {
		TwoFloats m_2f;
		bool m_bool;
		int m_int;
	};
};