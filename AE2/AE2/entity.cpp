#include "entity.h"

void Entity::Update()
{
	// If we have a melee attack
	if (m_melee_sphere)
	{
		// Update Time
		if (m_is_attacking)
		{
			float timeNow = float(timeGetTime() / 1000.0f);
			float deltaTime = timeNow - m_time_previous;
			m_time_previous = timeNow;
			m_attacking_timer += deltaTime;

			if (m_attacking_timer >= m_attack_time)
			{
				m_is_attacking = false;
				m_melee_sphere->SetActive(false);
				m_attacking_timer = 0.0f;
			}
		}
	}
}

void Entity::ChangeHealth(int amount)
{
	m_current_health += amount;

	if (m_current_health > m_max_health) m_current_health = m_max_health;
	else if (m_current_health < 0) SetActive(false); // DEATH
}

void Entity::MeleeAttack()
{
	if (m_melee_sphere)
	{
		m_is_attacking = true;
		m_melee_sphere->SetActive(true);
	}
}