#pragma once

#include "camera.h"
#include "objects.h"

class Player : public Camera
{
private:
	Spell* m_spell;
	float m_spell_time_previous = 0.0f;
	float m_spell_timer = 0.0f;
	float m_spell_time = 1.0f;
public:
	Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float x, float y, float z, float camera_rotation, float camera_pitch)
	{
		// CAMERA
		m_camera_rotation = camera_rotation;
		m_camera_pitch = camera_pitch;

		m_dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
		m_dy = (float)sin(m_camera_pitch * (XM_PI / 180.0));
		m_dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));

		// Jump
		m_jumping = false;
		m_jump_height = 10.0;
		m_jump_force = 0.005;
		m_gravity = 0.002;

		// MODEL
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

		// HEALTH SYSTEM
		m_current_health = m_max_health;
	}
	~Player() {}

	void Update() override;

	// Health System
	int GetMaxHealth() { return m_max_health; }
	void ChangeHealth(int amount);

	// Attack System
	void SpellAttack();
	void SetSpell(Spell* spell) { m_spell = spell; }
	Spell* GetSpell() { return m_spell; }

	bool CheckCanPush(Model* pushable);
};