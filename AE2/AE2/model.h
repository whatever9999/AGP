#pragma once

#include <d3d11.h>
#include <math.h>

#include "objfilemodel.h"

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

struct MODEL_CONSTANT_BUFFER // 64 bytes
{
	XMMATRIX WorldViewProjection;
};

struct MODEL_PIXEL_CONSTANT_BUFFER // 96 bytes
{
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	XMVECTOR point_light_position;
	XMVECTOR point_light_colour;
	XMFLOAT3 point_light_attenuation;
	float packing_1;
};

enum COLLISION_TYPE
{
	CONSTANT,
	PICKUP,
	TRIGGER,
	NUM_COLLISION_TYPES,
};

class Model
{
protected:
	ID3D11Device*			m_D3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVShader;
	ID3D11PixelShader*			m_pPShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11Buffer*				m_pPixelConstantBuffer;
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

	bool m_active = true;

	// Collision
	COLLISION_TYPE m_collision_type = CONSTANT;
	bool m_collision_started = false;

	char* m_vertex_shader;
	char* m_pixel_shader;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_xScale, m_yScale, m_zScale;

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
	Model() = default;
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
		m_xScale = 1.0f;
		m_yScale = 1.0f;
		m_zScale = 1.0f;
	}
	~Model()
	{
		if (m_pObject)
		{
			delete m_pObject;
			m_pObject = nullptr;
		}

		if (m_pTexture0)			m_pTexture0->Release();
		if (m_pTexture1)			m_pTexture1->Release();
		if (m_pSampler0)			m_pSampler0->Release();
		if (m_pPixelConstantBuffer)	m_pPixelConstantBuffer->Release();
		if (m_pConstantBuffer)		m_pConstantBuffer->Release();
		if (m_pVShader)				m_pVShader->Release();
		if (m_pInputLayout)			m_pInputLayout->Release();
		if (m_pPShader)				m_pPShader->Release();
	}

	virtual void Update() {};

	// Setters
	// Reset collision started when disabling/enabling
	void SetActive(bool active) { m_active = active; m_collision_started = false; }
	bool IsActive() { return m_active; }

	void SetCollisionType(COLLISION_TYPE type) { m_collision_type = type; }
	COLLISION_TYPE GetCollisionType() { return m_collision_type; }

	void SetX(float x) { m_x = x; }
	void SetY(float y) { m_y = y; }
	void SetZ(float z) { m_z = z; }
	void SetXAngle(float x_angle) { m_xAngle = x_angle; }
	void SetYAngle(float y_angle) { m_yAngle = y_angle; }
	void SetZAngle(float z_angle) { m_zAngle = z_angle; }
	void SetXScale(float scale) { m_xScale = scale; }
	void SetYScale(float scale) { m_yScale = scale; }
	void SetZScale(float scale) { m_zScale = scale; }
	// Getters
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	float GetZ() { return m_z; }
	float GetXAngle() { return m_xAngle; }
	float GetYAngle() { return m_yAngle; }
	float GetZAngle() { return m_zAngle; }
	float GetXScale() { return m_xScale; }
	float GetYScale() { return m_yScale; }
	float GetZScale() { return m_zScale; }

	// Orienting
	void LookAt_XZ(float x, float z);
	void MoveForward(float multiplier);

	// Collision
	// Use the largest radius
	float GetBoundingSphereRadius() { return m_bounding_sphere_radius * ((m_xScale > m_yScale) ? ((m_xScale > m_zScale) ? m_xScale : m_zScale) : ((m_yScale > m_zScale) ? m_yScale : m_zScale)); }
	XMVECTOR GetBoundingSphereWorldSpacePosition();
	bool CheckCollision(Model* other_model);
	virtual void OnCollision(Model* other_model) {}

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