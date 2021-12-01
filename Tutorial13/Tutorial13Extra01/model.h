#pragma once

#include <d3d11.h>
#include <math.h>

#include "objfilemodel.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

class Model
{
private:
	ID3D11Device*			m_D3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11ShaderResourceView*	m_pTexture1;
	ID3D11SamplerState*			m_pSampler0;

	XMVECTOR	m_directional_light_shines_from;
	XMVECTOR	m_directional_light_colour;
	XMVECTOR	m_ambient_light_colour;
	XMMATRIX	m_rotate_directional_light;
	XMVECTOR	m_point_light_position;
	XMVECTOR	m_point_light_colour;
	XMFLOAT3	m_point_light_attenuation;

	char* m_vertex_shader;
	char* m_pixel_shader;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;

	// Bounding Sphere Variables
	float m_bounding_sphere_centre_x;
	float m_bounding_sphere_centre_y;
	float m_bounding_sphere_centre_z;
	float m_bounding_sphere_radius;

	// Movement
	float m_speed;

	HRESULT Setup();
	
	void CalculateModelCentrePoint();
	void CalculateModelBoundingSphereRadius();

public:
	Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_scale = 1.0f;
	}
	~Model()
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

	// Setters
	void SetX(float x) { m_x = x; }
	void SetY(float y) { m_y = y; }
	void SetZ(float z) { m_z = z; }
	void SetXAngle(float x_angle) { m_xAngle = x_angle; }
	void SetYAngle(float y_angle) { m_yAngle = y_angle; }
	void SetZAngle(float z_angle) { m_zAngle = z_angle; }
	void SetScale(float scale) { m_scale = scale; }
	// Getters
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	float GetZ() { return m_z; }
	float GetXAngle() { return m_xAngle; }
	float GetYAngle() { return m_yAngle; }
	float GetZAngle() { return m_zAngle; }
	float GetScale() { return m_scale; }
	// Incrementors
	float IncX(float amount) { m_x += amount; }
	float IncY(float amount) { m_y += amount; }
	float IncZ(float amount) { m_z += amount; }
	float IncXAngle(float amount) { m_xAngle += amount; }
	float IncYAngle(float amount) { m_yAngle += amount; }
	float IncZAngle(float amount) { m_zAngle += amount; }
	float IncScale(float amount) { m_scale += amount; }

	// Orienting
	void LookAt_XZ(float x, float z);
	void MoveForward(float multiplier);

	// Collision
	float GetBoundingSphereRadius() { return m_bounding_sphere_radius * m_scale; }
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	bool CheckCollision(Model* other_model);

	// Movement
	void SetSpeed(float speed) { m_speed = speed; }
	float GetSpeed() { return m_speed; }

	HRESULT LoadObjModel(char* filename, char* pixel_shader, char* vertex_shader);

	void AddTextures(char* texture0_filename, char* texture1_filename);

	void AddDirectionalLight(XMVECTOR directional_light_shines_from, XMVECTOR directional_light_colour, XMMATRIX rotate_directional_light);
	void AddAmbientLight(XMVECTOR ambient_light_colour);
	void AddPointLight(XMVECTOR point_light_position, XMVECTOR point_light_colour, XMFLOAT3 point_light_attenuation);

	void Draw(XMMATRIX* view, XMMATRIX* projection);
};