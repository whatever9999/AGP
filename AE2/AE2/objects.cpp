#include "objects.h"

#include "entity.h"

// When the collision hits damage the enemy then disable the sphere (this will update the IsAttacking state of the player in Player::Update
void MeleeSphere::OnCollision(Model* other_model)
{
	if (!m_collision_started)
	{
		m_collision_started = true;

		Entity* enemy = static_cast<Entity*>(other_model);
		if (enemy)
		{
			enemy->ChangeHealth(-m_damage);
		}
	}
}