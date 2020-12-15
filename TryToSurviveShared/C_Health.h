#pragma once
#include "C_Base.h"
#include "Usings.h"

using Health = unsigned int;

class C_Health : public C_Base {
public:
	C_Health() : C_Base(Component::Health) {}
	void ReadIn(std::stringstream& l_stream) {
		l_stream >> m_maxHealth;
		m_health = m_maxHealth;
	}
	Health GetHealth()const { return m_health; }
	Health GetMaxHealth()const { return m_maxHealth; }
	void SetHealth(const Health& l_health) { m_health = l_health; }
	void ResetHealth() { m_health = m_maxHealth; }

//	UINT32 GetHurtDuration() { return m_hurtDuration; }
//	UINT32 GetDeathDuration() { return m_deathDuration; }
private:
	Health m_health;
	Health m_maxHealth;
//	UINT32 m_hurtDuration;
//	UINT32 m_deathDuration;
};