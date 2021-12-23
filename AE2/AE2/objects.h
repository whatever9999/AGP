#pragma once

#include "model.h"

class MeleeSphere : public Model
{
private:
	int m_damage = 25;
public:
	MeleeSphere(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_xAngle = -20.0f;
		m_yAngle = 0.0f;
		m_zAngle = 0.0f;
		m_xScale = 0.1f;
		m_yScale = 0.1f;
		m_zScale = 5.0f;
	}
	~MeleeSphere()
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

	void SetDamage(int damage) { m_damage = damage; }

	void OnCollision(Model* other_model) override;
};

class Spell : public Model
{
private:
	int m_damage = 50;
public:
	Spell(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
		m_xScale = 0.5f;
		m_yScale = 0.5f;
		m_zScale = 0.5f;
	}
	~Spell()
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

	void Update();

	void SetDamage(int damage) { m_damage = damage; }

	void OnCollision(Model* other_model) override;
};