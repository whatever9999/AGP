#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <DirectXMath.h>

#include "camera.h"
#include "text2D.h"
#include "sprite.h"
#include "objfilemodel.h"
#include "model.h"
#include "inputhandling.h"
#include "cube.h"

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
ID3D11DepthStencilView*		g_pZBuffer;
ID3D11BlendState*			g_pAlphaBlendEnable;
ID3D11BlendState*			g_pAlphaBlendDisable;

Camera*						g_camera;
Text2D*						g_2DText0;
Text2D*						g_2DText1;
Sprite*						g_Sprite;
vector<Model*>				g_Models;
InputHandling*				g_InputHandling;
Cube*						g_cube;

XMVECTOR g_directional_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;
XMMATRIX g_rotate_directional_light;
XMVECTOR g_point_light_position;
XMVECTOR g_point_light_colour;
XMFLOAT3 g_point_light_attenuation;

// Window Title
char		g_TutorialName[100] = "AGP\0";

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

#pragma region Initialisation
	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	g_InputHandling = new InputHandling();
	if (FAILED(g_InputHandling->InitialiseInput(g_hInst, g_hWnd)))
	{
		DXTRACE_MSG("Failed to initialise input");
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
#pragma endregion // Initialisation

	// Main Loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Handle Input
			g_InputHandling->ReadInputStates();
			g_InputHandling->HandleInput(g_hWnd, g_camera);

			// Update Camera
			g_camera->Update();

			// Render
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
	std::ZeroMemory(&sd, sizeof(sd));
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

	// Z Buffer Texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	std::ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

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
	std::ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	// Create Blend States for transparent text
	D3D11_BLEND_DESC b;
	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = FALSE;
	g_pD3DDevice->CreateBlendState(&b, &g_pAlphaBlendEnable);
	g_pD3DDevice->CreateBlendState(&b, &g_pAlphaBlendDisable);

	return S_OK;
}

void ShutdownD3D()
{
	if (g_cube)
	{
		delete g_cube;
		g_cube = nullptr;
	}
	if (g_InputHandling)
	{
		g_InputHandling->ShutdownInput();

		delete g_InputHandling;
		g_InputHandling = nullptr;
	}
	for (int i = 0; i < g_Models.size(); i++)
	{
		if (g_Models[i])
		{
			delete g_Models[i];
			g_Models[i] = nullptr;
		}
	}
	if (g_camera)
	{
		delete g_camera;
		g_camera = nullptr;
	}
	if (g_Sprite)
	{
		delete g_Sprite;
		g_Sprite = nullptr;
	}
	if (g_2DText0)
	{
		delete g_2DText0;
		g_2DText0 = nullptr;
	}
	if (g_2DText1)
	{
		delete g_2DText1;
		g_2DText1 = nullptr;
	}

	if (g_pAlphaBlendEnable)	g_pAlphaBlendEnable->Release();
	if (g_pAlphaBlendDisable)	g_pAlphaBlendDisable->Release();
	if (g_pZBuffer)				g_pZBuffer->Release();
	if (g_pBackBufferRTView)	g_pBackBufferRTView->Release();
	if (g_pSwapChain)			g_pSwapChain->Release();
	if (g_pImmediateContext)	g_pImmediateContext->Release();
	if (g_pD3DDevice)			g_pD3DDevice->Release();
}

HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;

	// Setup 2D Text
	g_2DText0 = new Text2D("assets/font1.png", g_pD3DDevice, g_pImmediateContext);
	g_2DText1 = new Text2D("assets/font2.png", g_pD3DDevice, g_pImmediateContext);

	// Setup UI Sprites
	g_Sprite = new Sprite("assets/UI.png", g_pD3DDevice, g_pImmediateContext);

	// Setup cube
	g_cube = new Cube(g_pD3DDevice, g_pImmediateContext);
	g_cube->LoadCube((char*)"assets/BoxTextureSmiley.bmp");

	// Add models to scene
	Model* model0 = new Model(g_pD3DDevice, g_pImmediateContext);
	model0->LoadObjModel((char*)"assets/PointySphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	model0->AddTextures((char*)"assets/BoxTexture.bmp", (char*)"assets/BoxTextureSmiley.bmp");
	model0->SetX(10);
	model0->SetScale(0.5);
	model0->SetSpeed(0.0002);

	Model* model1 = new Model(g_pD3DDevice, g_pImmediateContext);
	model1->LoadObjModel((char*)"assets/PointySphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	model1->AddTextures((char*)"assets/BoxTexture.bmp", (char*)"assets/BoxTextureSmiley.bmp");
	model1->SetX(-10);
	model1->SetSpeed(0.0001);

	g_Models.push_back(model0);
	g_Models.push_back(model1);

	// Create camera
	g_camera = new Camera(0.0, 0.0, -5.0, 0.0, 0.0);

	// Set directional light colour/direction
	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	// Set ambient light strength
	g_ambient_light_colour = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);

	// Set point light colour_pos and attenuation values
	g_point_light_position = XMVectorSet(5.0f, 5.0f, 15.0f, 0.0f);
	g_point_light_colour = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	// Don't set values to zero as we use them to divide
	g_point_light_attenuation = XMFLOAT3(0.1f, 0.2f, 0.1f);

	// Set directional light rotation vector
	g_rotate_directional_light = XMMatrixIdentity();

	return S_OK;
}

void CollisionCheck()
{
	// Check all models against each other to see if they've collided
	for (int i = 0; i < g_Models.size(); i++)
	{
		for (int j = 0; j < g_Models.size(); j++)
		{
			if (g_Models[i]->CheckCollision(g_Models[j]))
			{
				g_Models[i]->MoveForward(-1);
			}
		}
	}
}

void RenderFrame(void)
{
	// Clear the view
	float rgba_clear_colour[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Change directional light rotation
	g_rotate_directional_light *= XMMatrixRotationX(XMConvertToRadians(0.01f));
	g_rotate_directional_light *= XMMatrixRotationY(XMConvertToRadians(0.01f));
	g_rotate_directional_light *= XMMatrixRotationZ(XMConvertToRadians(0.01f));

	// World View Projection Matrix
	XMMATRIX projection, world, view;
	world = XMMatrixTranslation(0, 0, 0);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 640.0 / 480.0, 1.0, 100.0);
	view = g_camera->GetViewMatrix();

	// Show Cube
	g_cube->RenderCube(&view, &projection);

	// Show UI Sprites
	g_Sprite->RenderSprites();
	g_Sprite->AddBox(9, -1.0, 1.0, 0.1);
	g_Sprite->AddBox(8, -1.0, -0.9, 0.1);

	// Show 2D Text with transparent background
	g_2DText0->RenderText();
	g_2DText0->AddText("Hello world", -1.0, 1.0, 0.08);
	g_2DText1->RenderText();
	g_2DText1->AddText("Bye world!", -1.0, -0.9, 0.08);
	g_pImmediateContext->OMSetBlendState(g_pAlphaBlendDisable, 0, 0xffffffff);

	// Pointy sphere following model 1
	g_Models[0]->LookAt_XZ(g_Models[1]->GetX(), g_Models[1]->GetZ());
	g_Models[0]->MoveForward(1);
	CollisionCheck();
	g_Models[0]->AddAmbientLight(g_ambient_light_colour);
	g_Models[0]->AddDirectionalLight(g_directional_light_shines_from, g_directional_light_colour, g_rotate_directional_light);
	g_Models[0]->AddPointLight(g_point_light_position, g_point_light_colour, g_point_light_attenuation);
	g_Models[0]->Draw(&view, &projection);

	// Pointy sphere following model 0
	g_Models[1]->LookAt_XZ(g_Models[0]->GetX(), g_Models[0]->GetZ());
	g_Models[1]->MoveForward(1);
	CollisionCheck();
	g_Models[1]->AddAmbientLight(g_ambient_light_colour);
	g_Models[1]->AddDirectionalLight(g_directional_light_shines_from, g_directional_light_colour, g_rotate_directional_light);
	g_Models[1]->AddPointLight(g_point_light_position, g_point_light_colour, g_point_light_attenuation);
	g_Models[1]->Draw(&view, &projection);

	g_pSwapChain->Present(0, 0);
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