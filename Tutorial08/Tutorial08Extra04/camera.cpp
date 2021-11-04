#include "camera.h"

Camera::Camera(float x, float y, float z, float camera_rotation, float camera_pitch)
{
	m_x = x;
	m_y = y;
	m_z = z;
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
}
void Camera::Update()
{
	if (m_jumping)
	{
		m_y += m_jump_force;

		if (m_y > m_jump_height)
		{
			m_jumping = false;
		}
	}
	if (!m_jumping && m_y > 0)
	{
		m_y -= m_gravity;
	}
}

void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;

	m_dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
	m_dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));
}
void Camera::Pitch(float degrees)
{
	m_camera_pitch += degrees;

	if (m_camera_pitch > 90)
	{
		m_camera_pitch = 90;
	}
	else if (m_camera_pitch < -90)
	{
		m_camera_pitch = -90;
	}

	m_dy = (float)sin(m_camera_pitch * (XM_PI / 180.0));
}

void Camera::Forward(float distance)
{
	m_x += distance * m_dx;
	m_z += distance * m_dz;
}
void Camera::Up(float distance)
{
	m_y += distance;
}
void Camera::Strafe(float distance)
{
	XMVECTOR forward = XMVector3Normalize(m_lookat - m_position);

	XMVECTOR right = XMVector3Cross(forward, m_up);
	m_x += XMVectorGetX(right) * distance;
	m_z += XMVectorGetZ(right) * distance;
}

void Camera::Jump()
{
	m_jumping = true;
}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}