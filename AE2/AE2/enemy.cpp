#include "enemy.h"

void Enemy::Update()
{
	Entity::Update();

	// Update sight range position
	m_sight_range->SetX(m_x);
	m_sight_range->SetZ(m_z);
	
	// FSM
	State* m_previous_state = m_current_state;

	switch (m_current_state->GetState())
	{
	case PATROL:

		// If player is in sight range and alive then persue them
		if (m_sight_range->CheckCollision(m_player) && m_player->IsActive())
		{
			m_current_state = m_states[PERSUE];
		}
		break;
	case PERSUE:
		// If player is out of sight or dead then patrol
		if (!m_sight_range->CheckCollision(m_player) || !m_player->IsActive())
		{
			m_current_state = m_states[PATROL];
		}
		break;
	}

	// If we changed states run the enter/exit functions
	if (m_previous_state != m_current_state)
	{
		m_previous_state->Exit();
		m_current_state->Enter();
	}
	else
	{
		m_current_state->Update();
	}
}

void Enemy::MoveForward(float multiplier)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * m_speed * multiplier;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * m_speed * multiplier;

	// If the enemy tries to go out of bounds turn them and jump them back
	if (m_x > 50)
	{
		m_x = 49;
		m_yAngle += 90;
	}
	if (m_x < -50)
	{
		m_x = -49; 
		m_yAngle += 90;
	}
	if (m_z > 50)
	{
		m_z = 49; 
		m_yAngle += 90;
	}
	if (m_z < -50)
	{
		m_z = -49; 
		m_yAngle += 90;
	}
}

void PatrolState::Enter()
{
	// Ensure the walk timer is reset
	m_walk_timer = 0.0f;
}
void PatrolState::Update()
{
	// Rotate 90 degrees every m_walk_time seconds
	float timeNow = float(timeGetTime() / 1000.0f);
	float deltaTime = timeNow - m_time_previous;
	m_time_previous = timeNow;
	m_walk_timer += deltaTime;
	if (m_walk_time >= m_walk_timer)
	{
		m_enemy->SetYAngle(m_enemy->GetYAngle() + 90);
		m_walk_time = 0;
	}
	else
	{
		m_enemy->MoveForward(1);
	}
}

void PersueState::Update()
{
	m_enemy->LookAt_XZ(m_player->GetX(), m_player->GetZ());
	m_enemy->MoveForward(1);
}