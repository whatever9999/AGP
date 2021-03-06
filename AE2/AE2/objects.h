#pragma once

#include "reflectivemodel.h"

class MeleeSphere : public Model
{
private:
	int m_damage = 25;
public:
	MeleeSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_xAngle = -20.0f;
		m_yAngle = 0.0f;
		m_zAngle = 0.0f;
		m_xScale = 0.1f;
		m_yScale = 0.1f;
		m_zScale = 5.0f;
	}
	~MeleeSphere() {}

	void SetDamage(int damage) { m_damage = damage; }

	void OnCollision(Model* other_model) override;
};

class Spell : public Model
{
private:
	int m_damage = 50;
public:
	Spell(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_xScale = 0.5f;
		m_yScale = 0.5f;
		m_zScale = 0.5f;
	}
	~Spell() {}

	void Update();

	void SetDamage(int damage) { m_damage = damage; }

	void OnCollision(Model* other_model) override;
};

class CubeTrigger : public Model
{
private:
	bool m_triggered;
	Model* m_pushable_object;
public:
	CubeTrigger(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_xScale = 0.5f;
		m_yScale = 0.5f;
		m_zScale = 0.5f;
	}
	~CubeTrigger() {}

	void Update();

	bool CheckCollision(Model* other_model) override;

	void SetTriggered(int triggered) { m_triggered = triggered; }
	bool IsTriggered() { return m_triggered; }

	void OnCollision(Model* other_model) override;
};

class Door : public Model
{
private:
	vector<CubeTrigger*> m_triggers;
	bool m_unlocked;
	Model* m_player;
public:
	Door(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Model* player)
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
		m_xScale = 0.5f;
		m_yScale = 0.5f;
		m_zScale = 0.5f;
	}
	~Door() {}

	void Update();

	void OnCollision(Model* other_model) override;

	bool IsUnlocked() { return m_unlocked; }

	void AddCubeTrigger(CubeTrigger* trigger) { m_triggers.push_back(trigger); }
};

class LifePickup : public ReflectiveModel
{
private:
	int m_health_amount = 20;
	Model* m_player;
public:
	LifePickup(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Model* player)
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
		m_xScale = 0.5f;
		m_yScale = 0.5f;
		m_zScale = 0.5f;
	}
	~LifePickup() {}

	void Update();

	void OnCollision(Model* other_model) override;
};