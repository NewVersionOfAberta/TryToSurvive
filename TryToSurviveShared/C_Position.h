#pragma once
#pragma once

#include "C_Base.h"
#include "Usings.h"
#include <sstream>


class C_Position : public C_Base {
public:
	C_Position() : C_Base(Component::Position), m_elevation(0) {}
	~C_Position() {}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_position.first >> m_position.second >> m_elevation;
	}

	const sf::Vector2f& GetPosition() { return m_position; }
	const sf::Vector2f& GetOldPosition() { return m_positionOld; }
	unsigned int GetElevation() { return m_elevation; }

	void SetPosition(float l_x, float l_y) {
		//std::cout << "Position new: " << l_x << " " << l_y << std::endl;
		//std::cout << "Position old: " << l_x << " " << l_y << std::endl;
		m_positionOld = m_position;
		m_position = sf::Vector2f(l_x, l_y);
	}

	void SetPosition(const sf::Vector2f& l_vec) {
		//std::cout << "Position new: " << l_vec.first << " " << l_vec.second << std::endl;
		//std::cout << "Position old: " << m_position.first << " " << m_position.second << std::endl;
		m_positionOld = m_position;
		m_position = l_vec;
	}

	void SetElevation(unsigned int l_elevation) {
		m_elevation = l_elevation;
	}

	void MoveBy(float l_x, float l_y) {
		//std::cout << "Moved by: " << l_x << " : " << l_y << std::endl;
		m_positionOld = m_position;
		m_position.first += l_x;
		m_position.second += l_y;
	}

	void MoveBy(const sf::Vector2f& l_vec) {
		//std::cout << "Moved by: " << l_vec.first << " : " << l_vec.second << std::endl;
		m_positionOld = m_position;
		m_position.first += l_vec.first;
		m_position.second += l_vec.second;
	}

private:
	sf::Vector2f m_position;
	sf::Vector2f m_positionOld;
	unsigned int m_elevation;
};