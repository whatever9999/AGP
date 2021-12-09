#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

using namespace DirectX;

class Cube
{
private:
	ID3D11Device* m_D3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11VertexShader* m_pVShader;
	ID3D11PixelShader* m_pPShader;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState* m_pSampler0;

	HRESULT Setup();
public:
	Cube(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		m_D3DDevice = device;
		m_pImmediateContext = deviceContext;

		m_pVertexBuffer = nullptr;
		m_pVShader = nullptr;
		m_pPShader = nullptr;
		m_pInputLayout = nullptr;
		m_pConstantBuffer = nullptr;
		m_pTexture0 = nullptr;
		m_pSampler0 = nullptr;
	}
	~Cube()
	{
		if (m_pVertexBuffer)	m_pVertexBuffer->Release();
		if (m_pTexture0)		m_pTexture0->Release();
		if (m_pSampler0)		m_pSampler0->Release();
		if (m_pConstantBuffer)	m_pConstantBuffer->Release();
		if (m_pVShader)			m_pVShader->Release();
		if (m_pInputLayout)		m_pInputLayout->Release();
		if (m_pPShader)			m_pPShader->Release();
	}
	HRESULT LoadCube(char* filename);

	void RenderCube(XMMATRIX* view, XMMATRIX* projection);
};