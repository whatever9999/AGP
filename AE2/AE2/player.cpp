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

}