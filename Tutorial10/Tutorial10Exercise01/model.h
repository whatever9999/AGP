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

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;
	ID3D11PixelShader*		m_pPShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer;

	float m_x, m_y, m_z;
	float m_xAngle, m_yAngle, m_zAngle;
	float m_scale;

	HRESULT Setup();

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

	HRESULT LoadObjModel(char* filename);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
};