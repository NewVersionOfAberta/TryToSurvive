#pragma once
#include "C_Base.h"
#include "Directions.h"
#include "Usings.h"

class C_Movable : public C_Base {
public:
	C_Movable() : C_Base(Component::Movable),
		m_velocityMax(0.f), m_direction((Direction)0)
	{}

	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_velocityMax >> m_speed.first >> m_speed.second;

		unsigned int dir = 0;
		l_stream >> dir;
		m_direction = (Direction)dir;
	}

	const sf::Vector2f& GetVelocity() { return m_velocity; }
	float GetMaxVelocity() { return m_velocityMax; }
	const sf::Vector2f& GetSpeed() { return m_speed; }
	const sf::Vector2f& GetAcceleration() { return m_acceleration; }
	Direction GetDirection() { return m_direction; }

	void SetVelocity(const sf::Vector2f& l_vec) {
		//std::cout << "Velocity set new: " << l_vec.first << " : " << l_vec.second << std::endl;
		//std::cout << "Velocity set old: " << m_velocity.first << " : " << m_velocity.second << std::endl;
		m_velocity = l_vec; 
	}
	void SetMaxVelocity(float l_vel) { m_velocityMax = l_vel; }
	void SetSpeed(const sf::Vector2f& l_vec) { m_speed = l_vec; }
	void SetAcceleration(const sf::Vector2f& l_vec) { 
		//std::cout << "Acceleration new: " << l_vec.first << " : " << l_vec.second << std::endl;
		//std::cout << "Acceleration old: " << m_acceleration.first << " : " << m_acceleration.second << std::endl;
		m_acceleration = l_vec; 
	}
	void SetDirection(const Direction& l_dir) { m_direction = l_dir; }

	void AddVelocity(const sf::Vector2f& l_vec) {
		//std::cout << "Change by: " << l_vec.first << " : " << l_vec.second << std::endl;
		//std::cout << "Velocity add old: " << m_velocity.first << " : " << m_velocity.second << std::endl;
		m_velocity.first += l_vec.first;
		m_velocity.second += l_vec.second;
		if (std::abs(m_velocity.first) > m_velocityMax) {
			m_velocity.first = m_velocityMax * (m_velocity.first / std::abs(m_velocity.first));
		}

		if (std::abs(m_velocity.second) > m_velocityMax) {
			m_velocity.second = m_velocityMax * (m_velocity.second / std::abs(m_velocity.second));
		}
		//std::cout << "Velocity add new: " << m_velocity.first << " : " << m_velocity.second << std::endl;
	}

	void ApplyFriction(const sf::Vector2f& l_vec) {
		//std::cout << "Velocity apply friction: " << m_velocity.first << " : " << l_vec.second << std::endl;
		if (m_velocity.first != 0 && l_vec.first != 0) {
			if (std::abs(m_velocity.first) - std::abs(l_vec.first) < 0) {
				m_velocity.first = 0;
			}
			else {
				m_velocity.first += (m_velocity.first > 0 ? l_vec.first * -1 : l_vec.first);
			}
		}

		if (m_velocity.second != 0 && l_vec.second != 0) {
			if (std::abs(m_velocity.second) - std::abs(l_vec.second) < 0) {
				m_velocity.second = 0;
			}
			else {
				m_velocity.second += (m_velocity.second > 0 ? l_vec.second * -1 : l_vec.second);
			}
		}
		//std::cout << "Velocity apply friction new: " << m_velocity.first << " : " << m_velocity.second << std::endl;
	}

	void Accelerate(const sf::Vector2f& l_vec) 
	{ 
		m_acceleration.first += l_vec.first;
		m_acceleration.second += l_vec.second;
	}
	void Accelerate(float l_x, float l_y) 
	{ 
		m_acceleration.first += l_x;
		m_acceleration.second += l_y;
	}

	void Move(const Direction& l_dir) {
		
		if (l_dir == Direction::Up) {
			m_acceleration.second -= m_speed.second;
		}
		else if (l_dir == Direction::Down) {
			m_acceleration.second += m_speed.second;
		}
		else if (l_dir == Direction::Left) {
			m_acceleration.first -= m_speed.first;
		}
		else if (l_dir == Direction::Right) {
			m_acceleration.first += m_speed.first;
		}
		//std::cout << "Moved new acceleration: " << m_acceleration.first << " : " << m_acceleration.second << std::endl;
	}
private:
	sf::Vector2f m_velocity;
	float m_velocityMax;
	sf::Vector2f m_speed;
	sf::Vector2f m_acceleration;
	Direction m_direction;
};