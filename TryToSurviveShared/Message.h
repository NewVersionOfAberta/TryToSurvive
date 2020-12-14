#pragma once
using MessageType = unsigned int;
struct TwoFloats { 
	TwoFloats() : m_x(0.0), m_y(0.0){}
	float m_x; 
	float m_y; 
};

struct FourFloats {
	FourFloats(): p_x(0.0), p_y(0.0), d_x(0.0), d_y(0.0){}
		float p_x;
		float p_y;
		float d_x;
		float d_y;
};

struct Message {
	Message(const MessageType& l_type) : m_type(l_type), m_sender(0), m_receiver(0), m_2f() {}

	MessageType m_type;
	INT32 m_sender;
	INT32 m_receiver;

	union {
		TwoFloats m_2f;
		FourFloats m_4f;
		bool m_bool;
		int m_int;
	};
};