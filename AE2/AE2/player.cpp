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
	else if (m_current_health <= 0)
	{
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

bool Player::CheckCanPush(Model* pushable)
{
	// Work out which quadrant the player is facing
	float dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
	float dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));
	float yAngle = atan2(dx, dz) * (180.0 / XM_PI);
	if (yAngle < 0) yAngle += 360;

	float pushable_x = pushable->GetX();
	float pushable_z = pushable->GetZ();
	bool can_push = false;
	// If we're +-20 to 0, 90, 180 or 270 then we can push in 2 axes
	if (yAngle >= 340 || yAngle <= 20)
	{
		can_push = pushable_z > m_z;
	}
	else if (yAngle >= 70 && yAngle <= 110)
	{
		can_push = pushable_x > m_x;
	}
	else if (yAngle >= 160 && yAngle <= 200)
	{
		can_push = pushable_z < m_z;
	}
	else if (yAngle >= 250 && yAngle <= 290)
	{
		can_push = pushable_x < m_x;
	}
	// Otherwise, we can push the cube in the quadrant we're facing
	else
	{
		int quadrant = yAngle / 90;
		switch (quadrant)
		{
			// TOP RIGHT
		case 0:
			can_push = (pushable_x >= m_x && pushable_z >= m_z);
			break;
			// BOTTOM RIGHT
		case 1:
			can_push = (pushable_x >= m_x && pushable_z <= m_z);
			break;
			// BOTTOM LEFT
		case 2:
			can_push = (pushable_x <= m_x && pushable_z <= m_z);
			break;
			// TOP LEFT
		case 3:
			can_push = (pushable_x <= m_x && pushable_z >= m_z);
			break;
		}
	}

	return can_push;
}