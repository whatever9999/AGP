#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#include <vector>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

class Plane
{
private:
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11VertexShader* m_pVShader;
	ID3D11PixelShader* m_pPShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11Buffer* m_pPixelConstantBuffer;
	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState* m_pSampler0;

	XMVECTOR	m_directional_light_shines_from;
	XMVECTOR	m_directional_light_colour;
	XMVECTOR	m_ambient_light_colour;
	XMMATRIX	m_rotate_directional_light;
	// POINT LIGHTS
	std::vector<XMVECTOR> m_point_light_positions;
	std::vector<XMVECTOR> m_point_light_colours;
	std::vector<XMFLOAT3> m_point_light_attenuations;

	HRESULT Setup();
public:
	Plane(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_D3DDevice = device;
		m_pImmediateContext = deviceContext;

		m_pVertexBuffer = nullptr;
		m_pVShader = nullptr;
		m_pPShader = nullptr;
		m_pInputLayout = nullptr;
		m_pConstantBuffer = nullptr;
		m_pPixelConstantBuffer = nullptr;
		m_pTexture0 = nullptr;
		m_pSampler0 = nullptr;
	}
	~Plane()
	{
		if (m_pVertexBuffer)		m_pVertexBuffer->Release();
		if (m_pTexture0)			m_pTexture0->Release();
		if (m_pSampler0)			m_pSampler0->Release();
		if (m_pConstantBuffer)		m_pConstantBuffer->Release();
		if (m_pPixelConstantBuffer)	m_pPixelConstantBuffer->Release();
		if (m_pVShader)				m_pVShader->Release();
		if (m_pInputLayout)			m_pInputLayout->Release();
		if (m_pPShader)				m_pPShader->Release();
	}
	HRESULT LoadPlane(char* filename);

	void AddDirectionalLight(XMVECTOR directional_light_shines_from, XMVECTOR directional_light_colour, XMMATRIX rotate_directional_light);
	void AddAmbientLight(XMVECTOR ambient_light_colour);
	void AddPointLight(XMVECTOR point_light_position, XMVECTOR point_light_colour, XMFLOAT3 point_light_attenuation);
	void ClearPointLights();

	void RenderPlane(XMMATRIX* view, XMMATRIX* projection);
};