#include "enemy.h"

void Enemy::Update()
{
	Entity::Update();
	
	// FSM
	switch (m_current_state->GetState())
	{
	case PATROL:
		// If player is in sight range set state to persue
		break;
	case PERSUE:
		// If player is not in sight range set state to patrol
		// If player is in attack range set state to attack
		break;
	case ATTACK:
		// If player isn't in attack range set state to persue
		// If player isn't in sight range set state to patrol
		// If player is dead set state to patrol
		break;
	}

	m_current_state->Update();

}

void PatrolState::Update()
{

}

void PersueState::Update()
{

}

void AttackState::Update()
{

}