#pragma once

#include <d3d11.h>
#include <math.h>

#include "entity.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

class Camera : public Entity
{
protected:
	float m_dx;
	float m_dy;
	float m_dz;
	float m_camera_rotation;
	float m_camera_pitch;

	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;

	// Jump
	bool m_jumping;
	float m_jump_height;
	float m_jump_force;
	float m_gravity;

public:
	Camera() = default;
	Camera(float x, float y, float z, float camera_rotation, float camera_pitch);
	void Update();

	float GetX() { return m_x; }
	float GetY() { return m_y; }
	float GetZ() { return m_z; }

	void Rotate(float degrees);
	void Pitch(float degrees);

	void Forward(float distance);
	void Up(float distance);
	void Strafe(float distance);

	void Jump();

	XMMATRIX GetViewMatrix();
};