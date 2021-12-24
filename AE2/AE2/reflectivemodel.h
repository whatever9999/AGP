#pragma once

#include "model.h"

class ReflectiveModel : public Model
{
protected:
	HRESULT Setup();
public:
	ReflectiveModel() = default;
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
	~ReflectiveModel() {}

	HRESULT LoadObjModel(char* filename, char* pixel_shader, char* vertex_shader);

	void Draw(XMMATRIX* view, XMMATRIX* projection);
};