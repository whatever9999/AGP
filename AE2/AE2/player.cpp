#include "player.h"

void Player::Update()
{
	Camera::Update();

	// Melee Attack
	m_melee_sphere->SetX(m_x + 10);
	m_melee_sphere->SetY(m_y);
	m_melee_sphere->SetZ(m_z);
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