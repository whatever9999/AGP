#include "objects.h"

#include "entity.h"

// When the collision hits damage the enemy then disable the sphere (this will update the IsAttacking state of the player in Player::Update
void MeleeSphere::OnCollision(Model* other_model)
{
	if (!m_collision_started)
	{
		m_collision_started = true;

		// Has to be a dynamic cast so a Model can't be cast to an Entity unless it definitely is one
		Entity* enemy = dynamic_cast<Entity*>(other_model);
		if (enemy)
		{
			enemy->ChangeHealth(-m_damage);
		}
	}
}

void Spell::Update()
{
	MoveForward(1);
}
// When the spell hits an enemy disable it and damage the enemy
void Spell::OnCollision(Model* other_model)
{
	SetActive(false);

	// dynamic cast so spell only kills enemies
	Entity* enemy = dynamic_cast<Entity*>(other_model);
	if (enemy)
	{
		enemy->ChangeHealth(-m_damage);
	}
}

// If the pushable object comes off of the cube we need to disable its triggered state
void CubeTrigger::Update()
{
	if (m_pushable_object)
	{
		m_triggered = CheckCollision(m_pushable_object);
		if (!m_triggered)
		{
			m_pushable_object = nullptr;
			m_triggered = false;
		}
	}
}
void CubeTrigger::OnCollision(Model* other_model)
{
	if (other_model->GetCollisionType() == PUSHABLE)
	{
		m_pushable_object = other_model;
		m_triggered = true;
	}
}
// Collision check for cube trigger is smaller so it's less sensitive
bool CubeTrigger::CheckCollision(Model* other_model)
{
	if (other_model == this) return false;

	float x1 = XMVectorGetX(GetBoundingSphereWorldSpacePosition());
	float y1 = XMVectorGetY(GetBoundingSphereWorldSpacePosition());
	float z1 = XMVectorGetZ(GetBoundingSphereWorldSpacePosition());

	float x2 = XMVectorGetX(other_model->GetBoundingSphereWorldSpacePosition());
	float y2 = XMVectorGetY(other_model->GetBoundingSphereWorldSpacePosition());
	float z2 = XMVectorGetZ(other_model->GetBoundingSphereWorldSpacePosition());

	float distance_between_models = pow(x1 - x2, 2) +
		pow(y1 - y2, 2) +
		pow(z1 - z2, 2);

	if (distance_between_models < pow(GetBoundingSphereRadius()/4 + other_model->GetBoundingSphereRadius(), 2)) return true;
	else return false;
}

void Door::Update()
{
	bool was_unlocked = m_unlocked;
	m_unlocked = true;
	for (int i = 0; i < m_triggers.size(); i++)
	{
		if (!m_triggers[i]->IsTriggered())
		{
			m_unlocked = false;
			break;
		}
	}
	if (!was_unlocked && m_unlocked)
	{
		AddTextures((char*)"Assets/open_door.bmp", (char*)"Assets/open_door.bmp");
	}
	else if (was_unlocked && !m_unlocked)
	{
		AddTextures((char*)"Assets/door.bmp", (char*)"Assets/door.bmp");
	}
}
void Door::OnCollision(Model* other_model)
{
	if (m_unlocked && other_model == m_player)
	{
		m_player->SetActive(false);
	}
}

// Rotate for extra effect
void LifePickup::Update()
{
	m_yAngle += 0.1;
}
// Increase player health on pickup
void LifePickup::OnCollision(Model* other_model)
{
	if (other_model == m_player)
	{
		Entity* player = static_cast<Entity*>(m_player);
		player->ChangeHealth(m_health_amount);
		SetActive(false);
	}
}