#pragma once

#include "model.h"

#include <list>

struct Particle
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT4 color;
	float age;
};
const int numverts = 6;

class ParticleGenerator : public Model
{
protected:
	ID3D11Buffer* m_pVertexBuffer;

	ID3D11RasterizerState* m_pRasterSolid;
	ID3D11RasterizerState* m_pRasterParticle;

	float m_timePrevious;
	float m_untilParticleTimer;
	float m_age;
	float m_untilParticle;

	std::list<Particle*> m_free;
	std::list<Particle*> m_active;

	// If the particle system is running or not
	bool m_isActive;
	
	float RandomZeroToOne();
	float RandomNegOneToPosOne();

	void Update();
public:
	ParticleGenerator(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_isActive = true;

		// Set important values
		m_xScale = 0.3f;
		m_yScale = 0.3f;
		m_zScale = 0.3f;
		m_timePrevious = float(timeGetTime()/1000.0f);
		m_untilParticle = 0.001f;
		m_untilParticleTimer = m_untilParticle;
		m_age = 1.0f;

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
	}
	~ParticleGenerator()
	{
		while(m_free.size())
		{
			Particle* particle = m_free.front();
			m_free.pop_front();
			delete particle;
		}
		while (m_active.size())
		{
			Particle* particle = m_active.front();
			m_active.pop_front();
			delete particle;
		}

		if (m_pRasterSolid)			m_pRasterSolid->Release();
		if (m_pRasterParticle)		m_pRasterParticle->Release();
		if (m_pVertexBuffer)		m_pVertexBuffer->Release();
		if (m_pSampler0)			m_pSampler0->Release();
	}

	HRESULT Setup();

	void Draw(XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z);
	void DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z);
};