#include "camera.h"

Camera::Camera(float x, float y, float z, float camera_rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = camera_rotation;

	m_dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
	m_dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));
}

void Camera::Rotate(float degrees)
{
	m_camera_rotation += degrees;

	m_dx = (float)sin(m_camera_rotation * (XM_PI / 180.0));
	m_dz = (float)cos(m_camera_rotation * (XM_PI / 180.0));
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

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x + m_dx, m_y, m_x + m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}