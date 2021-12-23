#include "player.h"

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