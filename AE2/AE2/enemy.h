#pragma once

#include "entity.h"

enum STATE_TYPES
{
	PATROL,
	PERSUE,
	ATTACK,
	NUM_STATE_TYPES,
};

class State
{
protected:
	STATE_TYPES m_state;
public:
	STATE_TYPES GetState() { return m_state; }

	virtual void Enter() {};
	virtual void Update() {};
	virtual void Exit() {};
};

class PatrolState : public State
{
public:
	PatrolState()
	{
		m_state = PATROL;
	}

	void Update() override;
};

class PersueState : public State
{
public:
	PersueState()
	{
		m_state = PERSUE;
	}

	void Update() override;
};

class AttackState : public State
{
public:
	AttackState()
	{
		m_state = ATTACK;
	}

	void Update() override;
};

class Enemy : Entity
{
private:
	State* m_current_state;
	vector<State*> m_states;
public:
	Enemy(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_D3DDevice = device;
		m_pImmediateContext = deviceContext;

		m_pObject = nullptr;
		m_pVShader = nullptr;
		m_pPShader = nullptr;
		m_pInputLayout = nullptr;
		m_pConstantBuffer = nullptr;

		m_x = 0.0f;
		m_y = 0.0f;
		m_z = 0.0f;
		m_xAngle = 0.0f;
		m_yAngle = 0.0f;
		m_zAngle = 0.0f;
		m_xScale = 1.0f;
		m_yScale = 1.0f;
		m_zScale = 1.0f;

		// Health System
		m_current_health = m_max_health;

		// FSM
		PatrolState* patrol_state = new PatrolState();
		m_states.push_back(patrol_state);
		PersueState* persue_state = new PersueState();
		m_states.push_back(persue_state);
		AttackState* attack_state = new AttackState();
		m_states.push_back(attack_state);

		// Start in patrol state
		m_current_state = patrol_state;
	}
	~Enemy()
	{
		for (int i = 0; i < m_states.size(); i++)
		{
			if (m_states[i])
			{
				delete m_states[i];
				m_states[i] = nullptr;
			}
		}

		if (m_pObject)
		{
			delete m_pObject;
			m_pObject = nullptr;
		}

		if (m_pTexture0)		m_pTexture0->Release();
		if (m_pTexture1)		m_pTexture1->Release();
		if (m_pSampler0)		m_pSampler0->Release();
		if (m_pConstantBuffer)	m_pConstantBuffer->Release();
		if (m_pVShader)			m_pVShader->Release();
		if (m_pInputLayout)		m_pInputLayout->Release();
		if (m_pPShader)			m_pPShader->Release();
	}

	void Update() override;
};