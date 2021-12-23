#pragma once

#include "player.h"

// Forward Declaration
class Enemy;

enum STATE_TYPES
{
	PATROL,
	PERSUE,
	NUM_STATE_TYPES,
};

class State
{
protected:
	STATE_TYPES m_state;
	Enemy* m_enemy;
public:
	STATE_TYPES GetState() { return m_state; }

	virtual void Enter() {};
	virtual void Update() {};
	virtual void Exit() {};
};

class PatrolState : public State
{
private:
	float m_walk_timer = 0.0f;
	float m_walk_time = 4.0f;
	float m_time_previous = 0.0f;
public:
	PatrolState(Enemy* enemy)
	{
		m_state = PATROL;
		m_enemy = enemy;
	}

	void Enter() override;
	void Update() override;
};

class PersueState : public State
{
private:
	Player* m_player;
public:
	PersueState(Enemy* enemy, Player* player)
	{
		m_state = PERSUE;
		m_enemy = enemy;
		m_player = player;
	}

	void Update() override;
};


class Enemy : public Entity
{
private:
	State* m_current_state;
	vector<State*> m_states;

	Model* m_sight_range;

	Player* m_player;
public:
	Enemy(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Player* player)
	{
		m_player = player;

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
		PatrolState* patrol_state = new PatrolState(this);
		m_states.push_back(patrol_state);
		PersueState* persue_state = new PersueState(this, m_player);
		m_states.push_back(persue_state);

		// Start in patrol state
		m_current_state = patrol_state;

		// Setup melee sphere
		m_melee_sphere = new MeleeSphere(device, deviceContext);
		m_melee_sphere->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
		m_melee_sphere->AddTextures((char*)"assets/BoxTexture.bmp", (char*)"assets/BoxTexture.bmp");
		m_melee_sphere->SetCollisionType(TRIGGER);
		m_melee_sphere->SetActive(false);

		// Set up sight/attack range checks
		m_sight_range = new Model(device, deviceContext);
		m_sight_range->SetCollisionType(TRIGGER);
		m_sight_range->LoadObjModel((char*)"assets/Sphere.obj", (char*)"ModelPS", (char*)"ModelVS");
		m_sight_range->SetX(m_x);
		m_sight_range->SetXScale(10.0f);
		m_sight_range->SetY(m_y);
		m_sight_range->SetYScale(10.0f);
		m_sight_range->SetZ(m_z);
		m_sight_range->SetZScale(10.0f);
	}
	~Enemy()
	{
		if (m_sight_range)
		{
			delete m_sight_range;
			m_sight_range = nullptr;
		}

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

