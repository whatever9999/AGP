#pragma once

#include "model.h"

struct Particle
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT4 color;
};
const int numverts = 6;

class ParticleGenerator : public Model
{
protected:
	ID3D11Buffer* m_pVertexBuffer;

	ID3D11RasterizerState* m_pRasterSolid = 0;
	ID3D11RasterizerState* m_pRasterParticle = 0;
public:
	ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_D3DDevice = device;
		m_pImmediateContext = deviceContext;

		m_pVertexBuffer = nullptr;
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
	~ParticleGenerator()
	{
		if (m_pObject)
		{
			delete m_pObject;
			m_pObject = nullptr;
		}

		if (m_pRasterSolid != 0)	m_pRasterSolid->Release();
		if (m_pRasterParticle != 0)	m_pRasterParticle->Release();
		if (m_pVertexBuffer)		m_pVertexBuffer->Release();
		if (m_pTexture0)			m_pTexture0->Release();
		if (m_pTexture1)			m_pTexture1->Release();
		if (m_pSampler0)			m_pSampler0->Release();
		if (m_pConstantBuffer)		m_pConstantBuffer->Release();
		if (m_pVShader)				m_pVShader->Release();
		if (m_pInputLayout)			m_pInputLayout->Release();
		if (m_pPShader)				m_pPShader->Release();
	}

	HRESULT Setup();

	void Draw(XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z);
	void DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z);
};