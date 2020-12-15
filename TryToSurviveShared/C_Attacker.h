#pragma once
#include "C_TimedComponentBase.h"

class C_Attacker : public C_TimedComponentBase{
public:
	C_Attacker(): C_TimedComponentBase(Component::Attacker), 
		m_attacked(false), m_attackDuration(0){}
	void ReadIn(std::stringstream& l_stream){
		l_stream >> m_offset.first >> m_offset.second 
			>> m_attackArea.width >> m_attackArea.height 
			 >> m_attackDuration;
	}

	void SetSize(const sf::Vector2f& l_size){
		m_attackArea.width = l_size.first;
		m_attackArea.height = l_size.second;

	}

	void SetAreaPosition(const sf::Vector2f& l_pos){
		m_attackArea.left = l_pos.first;
		m_attackArea.top = l_pos.second;
		
	}

	const sf::FloatRect& GetAreaOfAttack(){ return m_attackArea; }
	const sf::Vector2f& GetOffset(){ return m_offset; }
	bool HasAttacked(){ return m_attacked; }
	void SetAttacked(bool l_attacked){ m_attacked = l_attacked; }
	void SetOwner(int l_owner) { m_owner = l_owner; }
	int GetOwner() { return m_owner; }
	//float GetKnockback(){ return m_knockback; }
	//sf::Uint32 GetAttackDuration(){ return m_attackDuration; }
private:
	sf::FloatRect m_attackArea;
	sf::Vector2f m_offset;
	bool m_attacked;
	int m_owner;
	sf::Uint32 m_attackDuration;
};