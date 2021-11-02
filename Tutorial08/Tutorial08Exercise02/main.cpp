#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include "camera.h"

using namespace DirectX;

// Global Vars
HINSTANCE	g_hInst = NULL;
HWND		g_hWnd	= NULL;

D3D_DRIVER_TYPE				g_driverType			= D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel			= D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice			= NULL;
ID3D11DeviceContext*		g_pImmediateContext		= NULL;
IDXGISwapChain*				g_pSwapChain			= NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView		= NULL;
ID3D11Buffer*				g_pVertexBuffer;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;
ID3D11InputLayout*			g_pInputLayout;
ID3D11Buffer*				g_pConstantBuffer0;
ID3D11DepthStencilView*		g_pZBuffer;
ID3D11ShaderResourceView*	g_pTexture0;
ID3D11SamplerState*			g_pSampler0;

Camera*						g_camera;

struct POS_COL_TEX_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
};

struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;
	float RedAmount;
	float Scale;
	XMFLOAT2 packing_bytes;
};

CONSTANT_BUFFER0 g_cb0_values;
float g_world_z = 0;
float g_degrees;

const int numVertices = 36;

// Window Title
char		g_TutorialName[100] = "Lab 03 Exercise 01\0";

// Forward Declarations
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseD3D();
void	ShutdownD3D();
void	RenderFrame(void);
void	ResizeWindow(UINT new_size[2]);
HRESULT InitialiseGraphics(void);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	MSG msg = { 0 };

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_camera->Update();
			g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &g_cb0_values, 0, 0);
			RenderFrame();
		}
	}

	ShutdownD3D();

	return (int)msg.wParam;
}

HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	char Name[100] = "Dominique Russell\0";

	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wcex.hbrBackground = (HBRUSH)(COLOUR_WINDOW + 1);
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!g_hWnd)
	{
		return E_FAIL;
	}

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_SPACE:
					g_camera->Jump();
					break;
				case VK_ESCAPE:
					DestroyWindow(g_hWnd);
					return 0;
				case VK_UP:
					g_camera->Pitch(1);
					break;
				case VK_DOWN:
					g_camera->Pitch(-1);
					break;
			}
			break;
		case WM_CHAR:
			switch (wParam)
			{
				case 'w':
					g_camera->Forward(1);
					break;
				case 's':
					g_camera->Forward(-1);
					break;
				case 'z':
					g_camera->Up(1);
					break;
				case 'x':
					g_camera->Up(-1);
					break;
				case 'q':
					g_camera->Rotate(-1);
					break;
				case 'e':
					g_camera->Rotate(1);
					break;
				case 'd':
					g_camera->Strafe(-1);
					break;
				case 'a':
					g_camera->Strafe(1);
					break;
			}
			break;
		case WM_SIZE:
			if (g_pSwapChain)
			{
				UINT new_size[2];
				new_size[0] = LOWORD(lParam);
				new_size[1] = HIWORD(lParam);

				ResizeWindow(new_size);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(	NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
											D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pD3DDevice, &g_featureLevel, &g_pImmediateContext	);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		return hr;
	}

	ID3D11Texture2D* pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Z Buffer Texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();


	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	return S_OK;
}

void ShutdownD3D()
{
	delete g_camera;

	if (g_pTexture0)			g_pTexture0->Release();
	if (g_pSampler0)			g_pSampler0->Release();
	if (g_pZBuffer)				g_pZBuffer->Release();
	if (g_pConstantBuffer0)		g_pConstantBuffer0->Release();
	if (g_pVertexBuffer)		g_pVertexBuffer->Release();
	if (g_pInputLayout)			g_pInputLayout->Release();
	if (g_pVertexShader)		g_pVertexShader->Release();
	if (g_pPixelShader)			g_pPixelShader->Release();
	if (g_pBackBufferRTView)	g_pBackBufferRTView->Release();
	if (g_pSwapChain)			g_pSwapChain->Release();
	if (g_pImmediateContext)	g_pImmediateContext->Release();
	if (g_pD3DDevice)			g_pD3DDevice->Release();
}

HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;

	POS_COL_TEX_VERTEX vertices[] =
	{
		// back face
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},

		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		// front face
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},

		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		// left face
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		// right face
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},

		// bottom face
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f)},

		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f)},

		// top face
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f)},

		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
	};

	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	// Create Constant Buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 80;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_MAPPED_SUBRESOURCE ms;

	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	memcpy(ms.pData, vertices, sizeof(vertices));

	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	ID3DBlob* VS, * PS, * error;

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	// Connect constant buffer
	g_cb0_values.RedAmount = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	g_cb0_values.Scale = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &g_cb0_values, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);

	// Create camera
	g_camera = new Camera(0.0, 0.0, -5.0, 0.0, 0.0);

	// Setup Shader Resource View
	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "assets/BoxTexture.bmp", NULL, NULL, &g_pTexture0, NULL);

	// Setup Sampler
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	g_pD3DDevice->CreateSamplerState(&sampler_desc, &g_pSampler0);

	return S_OK;
}

void RenderFrame(void)
{
	float rgba_clear_colour[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UINT stride = sizeof(POS_COL_TEX_VERTEX);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSampler0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexture0);

	g_pImmediateContext->Draw(numVertices, 0);

	g_pSwapChain->Present(0, 0);


	// World View Projection Matrix
	XMMATRIX projection, world, view;

	world = XMMatrixRotationX(XMConvertToRadians(g_degrees));
	world *= XMMatrixRotationY(XMConvertToRadians(g_degrees));
	world *= XMMatrixRotationZ(XMConvertToRadians(g_degrees));
	world *= XMMatrixTranslation(0, 0, 10 + g_world_z);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 640.0 / 480.0, 1.0, 100.0);
	view = g_camera->GetViewMatrix();
	g_cb0_values.WorldViewProjection = world * view * projection;
}

void ResizeWindow(UINT new_size[2])
{
	HRESULT hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Release all outstanding references to the swap chain's buffers.
	g_pBackBufferRTView->Release();

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.
	hr = g_pSwapChain->ResizeBuffers(1, new_size[0], new_size[1], DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	if (FAILED(hr)) abort();

	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) abort();

	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &g_pBackBufferRTView);
	
	if (FAILED(hr)) abort();

	pBackBufferTexture->Release();

	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)new_size[0];
	viewport.Height = (FLOAT)new_size[1];
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	g_pImmediateContext->RSSetViewports(1, &viewport);
}