#include "player.h"

void Player::Update()
{
	Camera::Update();

	// Melee Attack
	m_melee_sphere->SetX(m_x);
	m_melee_sphere->SetY(m_y - 1.0f);
	m_melee_sphere->SetZ(m_z);

	// Angle the sword according to the camera rotation
	float dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
	float dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));
	float swordYAngle = atan2(dx, dz) * (180.0 / XM_PI);
	m_melee_sphere->SetYAngle(swordYAngle);

	// Null reference check for spell
	if (m_spell)
	{
		// Disable the spell if it's out of time
		if (m_spell->IsActive())
		{
			float timeNow = float(timeGetTime() / 1000.0f);
			float deltaTime = timeNow - m_spell_time_previous;
			m_spell_time_previous = timeNow;
			m_spell_timer += deltaTime;

			if (m_spell_timer >= m_spell_time)
			{
				m_spell->SetActive(false);
				m_spell_timer = 0.0f;
			}
		}
		else
		{
			// Angle the spell according to the camera rotation
			// Only do this if the spell isn't active as we don't want it to move while shooting
			m_spell->SetYAngle(swordYAngle);
		}
	}

	Entity::Update();
}

// Override so that when player dies the game ends
void Player::ChangeHealth(int amount)
{
	m_current_health += amount;

	if (m_current_health > m_max_health) m_current_health = m_max_health;
	else if (m_current_health < 0)
	{
		// TODO: Game Over Screen
		// Disable Player
		SetActive(false);
	}
}

void Player::SpellAttack()
{
	// The spell comes from the player's position
	m_spell->SetX(m_x);
	m_spell->SetY(m_y);
	m_spell->SetZ(m_z);
	m_spell->SetActive(true);
}