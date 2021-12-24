#pragma once

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
#include "reflectivemodel.h"
#include "inputhandling.h"
#include "skybox.h"
#include "particlegenerator.h"
#include "plane.h"
#include "player.h"

using namespace DirectX;

class Game
{
private:
	// Setup
	HINSTANCE					m_hInst = NULL;
	HWND						m_hWnd = NULL;

	D3D_DRIVER_TYPE				m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL			m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*				m_pD3DDevice = NULL;
	ID3D11DeviceContext*		m_pImmediateContext = NULL;
	IDXGISwapChain*				m_pSwapChain = NULL;
	ID3D11RenderTargetView*		m_pBackBufferRTView = NULL;
	ID3D11DepthStencilView*		m_pZBuffer;
	ID3D11BlendState*			m_pAlphaBlendEnable;
	ID3D11BlendState*			m_pAlphaBlendDisable;

	// Game elements
	Player*						m_player;
	Text2D*						m_2DText0;
	Text2D*						m_2DText1;
	Sprite*						m_Sprite;
	vector<Model*>				m_Models;
	InputHandling*				m_InputHandling;
	Skybox*						m_skybox;
	ParticleGenerator*			m_particleGenerator;
	Plane*						m_plane;

	// Lighting
	XMVECTOR					m_directional_light_shines_from;
	XMVECTOR					m_directional_light_colour;
	XMVECTOR					m_ambient_light_colour;
	XMMATRIX					m_rotate_directional_light;
	// POINT LIGHTS
	XMVECTOR					m_point_light0_position;
	XMVECTOR					m_point_light0_colour;
	XMFLOAT3					m_point_light0_attenuation;

	XMVECTOR					m_point_light1_position;
	XMVECTOR					m_point_light1_colour;
	XMFLOAT3					m_point_light1_attenuation;

	XMVECTOR					m_point_light2_position;
	XMVECTOR					m_point_light2_colour;
	XMFLOAT3					m_point_light2_attenuation;

	XMVECTOR					m_point_light3_position;
	XMVECTOR					m_point_light3_colour;
	XMFLOAT3					m_point_light3_attenuation;

	XMVECTOR					m_point_light4_position;
	XMVECTOR					m_point_light4_colour;
	XMFLOAT3					m_point_light4_attenuation;
public:
	HRESULT InitialiseD3D();
	HRESULT InitialiseGame();

	void GameLoop();
	void ShutdownD3D();
	void CollisionCheck();
	void RenderFrame(void);
	void ResizeWindow(UINT new_size[2]);

	// Getters and Setters
	InputHandling* GetInputHandling() { return m_InputHandling; }
	void SetInputHandling(InputHandling* inputHandling) { m_InputHandling = inputHandling; }
	
	HINSTANCE GetInst() { return m_hInst; }
	void SetInst(HINSTANCE inst) { m_hInst = inst; }

	HWND GetWnd() { return m_hWnd; };
	void SetWnd(HWND wnd) { m_hWnd = wnd; }

	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
};