#pragma once

#include "model.h"
#include "objects.h"

class Entity : public Model
{
protected:
	// Health System
	int m_current_health;
	int m_max_health = 100;

	// Attack System
	MeleeSphere* m_melee_sphere;
	bool m_is_attacking = false;
	float m_attacking_timer = 0.0f;
	float m_attack_time = 1.0f;
	float m_time_previous = 0.0f;
public:
	Entity() = default;
	Entity(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
	}
	~Entity()
	{
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

	int GetHealth() { return m_current_health; }
	void ChangeHealth(int amount);

	// Attack System
	bool IsAttacking() { return m_is_attacking; }
	void MeleeAttack();
	void SetMeleeSphere(MeleeSphere* melee_sphere) { m_melee_sphere = melee_sphere; }
	MeleeSphere* GetMeleeSphere() { return m_melee_sphere; }
};