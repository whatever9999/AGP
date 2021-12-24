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

class Skybox
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

	ID3D11RasterizerState* m_pRasterSolid = 0;
	ID3D11RasterizerState* m_pRasterSkyBox = 0;
	ID3D11DepthStencilState* m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState* m_pDepthWriteSkybox = 0;

	HRESULT Setup();
public:
	Skybox(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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
	~Skybox()
	{
		if (m_pRasterSolid)				m_pRasterSolid->Release();
		if (m_pRasterSkyBox)			m_pRasterSkyBox->Release();
		if (m_pDepthWriteSolid)			m_pDepthWriteSolid->Release();
		if (m_pDepthWriteSkybox)		m_pDepthWriteSkybox->Release();
		if (m_pVertexBuffer)			m_pVertexBuffer->Release();
		if (m_pTexture0)				m_pTexture0->Release();
		if (m_pSampler0)				m_pSampler0->Release();
		if (m_pConstantBuffer)			m_pConstantBuffer->Release();
		if (m_pVShader)					m_pVShader->Release();
		if (m_pInputLayout)				m_pInputLayout->Release();
		if (m_pPShader)					m_pPShader->Release();
	}
	HRESULT LoadSkybox(char* filename);

	void RenderSkybox(XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z);
};