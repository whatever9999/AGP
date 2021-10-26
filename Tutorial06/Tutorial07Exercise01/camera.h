#pragma once

#include <d3d11.h>
#include <math.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	float m_x;
	float m_y;
	float m_z;
	float m_dx;
	float m_dz;
	float m_camera_rotation;

	XMVECTOR m_position;
	XMVECTOR m_lookat;
	XMVECTOR m_up;

public:
	Camera(float x, float y, float z, float camera_rotation);
	void Rotate(float degrees);
	void Forward(float distance);
	XMMATRIX GetViewMatrix();
};