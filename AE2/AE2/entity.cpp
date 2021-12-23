#include "entity.h"

void Entity::ChangeHealth(int amount)
{
	m_current_health += amount;

	if (m_current_health > m_max_health) m_current_health = m_max_health;
	else if (m_current_health < 0) SetActive(false); // DEATH
}