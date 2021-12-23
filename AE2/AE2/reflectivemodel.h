#pragma once

#include "model.h"

class ReflectiveModel : public Model
{
protected:
	HRESULT Setup();
public:
	ReflectiveModel(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
	~ReflectiveModel()
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

	HRESULT LoadObjModel(char* filename, char* pixel_shader, char* vertex_shader);

	void Draw(XMMATRIX* view, XMMATRIX* projection);
};