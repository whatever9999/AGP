#include "game.h"

#include "enemy.h"

HRESULT Game::InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
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
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pD3DDevice, &m_featureLevel, &m_pImmediateContext);
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
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	hr = m_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pBackBufferRTView);
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
	hr = m_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	std::ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &m_pZBuffer);
	pZBufferTexture->Release();

	m_pImmediateContext->OMSetRenderTargets(1, &m_pBackBufferRTView, m_pZBuffer);

	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &viewport);

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
	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendEnable);
	m_pD3DDevice->CreateBlendState(&b, &m_pAlphaBlendDisable);

	return S_OK;
}
HRESULT Game::InitialiseGame()
{
	HRESULT hr = S_OK;

	// Create player
	m_player = new Player(m_pD3DDevice, m_pImmediateContext, 0.0, 0.0, -5.0, 0.0, 0.0);
	m_player->LoadObjModel((char*)"assets/Sphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	m_player->SetX(-40);
	m_player->SetZ(-40);

	// Add plane
	m_plane = new Plane(m_pD3DDevice, m_pImmediateContext);
	m_plane->LoadPlane((char*)"assets/Plane.bmp");

	// Setup 2D Text
	m_2DText0 = new Text2D("assets/font2.png", m_pD3DDevice, m_pImmediateContext);
	m_2DText1 = new Text2D("assets/font2.png", m_pD3DDevice, m_pImmediateContext);

	// Setup UI Sprites
	m_Sprite = new Sprite("assets/UI.png", m_pD3DDevice, m_pImmediateContext);

	// Setup skybox
	m_skybox = new Skybox(m_pD3DDevice, m_pImmediateContext);
	m_skybox->LoadSkybox((char*)"assets/skybox02.dds");

	// Create particle generator
	m_particleGenerator = new ParticleGenerator(m_pD3DDevice, m_pImmediateContext);
	m_particleGenerator->Setup();

	// Player Melee
	MeleeSphere* meleeSphere = new MeleeSphere(m_pD3DDevice, m_pImmediateContext);
	meleeSphere->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	meleeSphere->AddTextures((char*)"assets/sword.bmp", (char*)"assets/sword.bmp");
	meleeSphere->SetCollisionType(TRIGGER);
	meleeSphere->SetActive(false);
	m_player->SetMeleeSphere(meleeSphere);

	// Player Spell
	Spell* spell = new Spell(m_pD3DDevice, m_pImmediateContext);
	spell->LoadObjModel((char*)"assets/Sphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	spell->AddTextures((char*)"assets/spell.bmp", (char*)"assets/spell.bmp");
	spell->SetCollisionType(TRIGGER);
	spell->SetActive(false);
	spell->SetSpeed(0.05);
	m_player->SetSpell(spell);

#pragma region Enemies
	Enemy* enemy0 = new Enemy(m_pD3DDevice, m_pImmediateContext, m_player);
	enemy0->LoadObjModel((char*)"assets/PointySphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	enemy0->AddTextures((char*)"assets/enemy.bmp", (char*)"assets/enemy.bmp");
	enemy0->SetX(-40);
	enemy0->SetZ(-20);
	enemy0->SetXScale(0.5);
	enemy0->SetYScale(0.5);
	enemy0->SetZScale(0.5);
	enemy0->SetSpeed(0.003);

	Enemy* enemy1 = new Enemy(m_pD3DDevice, m_pImmediateContext, m_player);
	enemy1->LoadObjModel((char*)"assets/PointySphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	enemy1->AddTextures((char*)"assets/enemy.bmp", (char*)"assets/enemy.bmp");
	enemy1->SetX(40);
	enemy1->SetZ(40);
	enemy1->SetXScale(0.5);
	enemy1->SetYScale(0.5);
	enemy1->SetZScale(0.5);
	enemy1->SetSpeed(0.002);

	Enemy* enemy2 = new Enemy(m_pD3DDevice, m_pImmediateContext, m_player);
	enemy2->LoadObjModel((char*)"assets/PointySphere.obj", (char*)"ModelPS", (char*)"ModelVS");
	enemy2->AddTextures((char*)"assets/enemy.bmp", (char*)"assets/enemy.bmp");
	enemy2->SetX(20);
	enemy2->SetZ(-40);
	enemy2->SetXScale(0.5);
	enemy2->SetYScale(0.5);
	enemy2->SetZScale(0.5);
	enemy2->SetSpeed(0.002);
#pragma endregion

#pragma region Pushable Cubes
	Model* pushable_cube0 = new Model(m_pD3DDevice, m_pImmediateContext);
	pushable_cube0->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	pushable_cube0->AddTextures((char*)"assets/BoxTexture.bmp", (char*)"assets/BoxTexture.bmp");
	pushable_cube0->SetX(0);
	pushable_cube0->SetZ(40);
	pushable_cube0->SetY(-3.8);
	pushable_cube0->SetXScale(3.0);
	pushable_cube0->SetYScale(3.0);
	pushable_cube0->SetZScale(3.0);
	pushable_cube0->SetSpeed(0.001);
	pushable_cube0->SetCollisionType(PUSHABLE);

	Model* pushable_cube1 = new Model(m_pD3DDevice, m_pImmediateContext);
	pushable_cube1->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	pushable_cube1->AddTextures((char*)"assets/BoxTexture.bmp", (char*)"assets/BoxTexture.bmp");
	pushable_cube1->SetX(40);
	pushable_cube1->SetZ(0);
	pushable_cube1->SetY(-3.8);
	pushable_cube1->SetXScale(3.0);
	pushable_cube1->SetYScale(3.0);
	pushable_cube1->SetZScale(3.0);
	pushable_cube1->SetSpeed(0.001);
	pushable_cube1->SetCollisionType(PUSHABLE);
#pragma endregion

#pragma region Cube Triggers
	CubeTrigger* cube_trigger0 = new CubeTrigger(m_pD3DDevice, m_pImmediateContext);
	cube_trigger0->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	cube_trigger0->AddTextures((char*)"assets/cube_trigger.bmp", (char*)"assets/cube_trigger.bmp");
	cube_trigger0->SetX(-15);
	cube_trigger0->SetZ(40);
	cube_trigger0->SetY(-7);
	cube_trigger0->SetXScale(3.0);
	cube_trigger0->SetYScale(0.1);
	cube_trigger0->SetZScale(3.0);
	cube_trigger0->SetCollisionType(TRIGGER);

	CubeTrigger* cube_trigger1 = new CubeTrigger(m_pD3DDevice, m_pImmediateContext);
	cube_trigger1->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	cube_trigger1->AddTextures((char*)"assets/cube_trigger.bmp", (char*)"assets/cube_trigger.bmp");
	cube_trigger1->SetX(40);
	cube_trigger1->SetZ(0);
	cube_trigger1->SetY(-7);
	cube_trigger1->SetXScale(3.0);
	cube_trigger1->SetYScale(0.1);
	cube_trigger1->SetZScale(3.0);
	cube_trigger1->SetCollisionType(TRIGGER);
#pragma endregion

#pragma region Door
	Door* door = new Door(m_pD3DDevice, m_pImmediateContext, m_player);
	door->LoadObjModel((char*)"assets/Cube.obj", (char*)"ModelPS", (char*)"ModelVS");
	door->AddTextures((char*)"assets/door.bmp", (char*)"assets/door.bmp");
	door->SetX(40);
	door->SetZ(-40);
	door->SetY(-1.2);
	door->SetXScale(6.0);
	door->SetYScale(6.0);
	door->SetZScale(6.0);
	door->SetCollisionType(CONSTANT);
	door->AddCubeTrigger(cube_trigger0);
	door->AddCubeTrigger(cube_trigger1);
#pragma endregion

#pragma region Life Pickups
	LifePickup* life_pickup0 = new LifePickup(m_pD3DDevice, m_pImmediateContext, m_player);
	life_pickup0->LoadObjModel((char*)"assets/Cube.obj", (char*)"ReflectiveModelPS", (char*)"ReflectiveModelVS");
	life_pickup0->AddTextures((char*)"assets/skybox02.dds", (char*)"assets/skybox02.dds");
	life_pickup0->SetX(-30);
	life_pickup0->SetZ(30);
	life_pickup0->SetY(-3);
	life_pickup0->SetCollisionType(PICKUP);

	LifePickup* life_pickup1 = new LifePickup(m_pD3DDevice, m_pImmediateContext, m_player);
	life_pickup1->LoadObjModel((char*)"assets/Cube.obj", (char*)"ReflectiveModelPS", (char*)"ReflectiveModelVS");
	life_pickup1->AddTextures((char*)"assets/skybox02.dds", (char*)"assets/skybox02.dds");
	life_pickup1->SetX(40);
	life_pickup1->SetZ(-20);
	life_pickup1->SetY(-3);
	life_pickup1->SetCollisionType(PICKUP);
#pragma endregion
	// Set directional light colour/direction (according to skybox)
	m_directional_light_shines_from = XMVectorSet(-1.0f, 5.0f, -0.5f, 0.0f);
	m_directional_light_colour = XMVectorSet(2.0f, 2.0f, 2.0f, 0.0f);

	// Set ambient light strength
	m_ambient_light_colour = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);

#pragma region Point Lights
	// Set point light colour_pos and attenuation values
	m_point_light0_colour = XMVectorSet(0.0f, 0.0f, 0.01f, 1.0f);
	// Don't set values to zero as we use them to divide
	m_point_light0_attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_point_light1_colour = XMVectorSet(0.01f, 0.0f, 0.0f, 1.0f);
	m_point_light1_attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_point_light2_colour = XMVectorSet(0.01f, 0.0f, 0.0f, 1.0f);
	m_point_light2_attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_point_light3_colour = XMVectorSet(0.0f, 0.0f, 0.01f, 1.0f);
	m_point_light3_attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_point_light4_colour = XMVectorSet(0.0f, 0.0f, 0.01f, 1.0f);
	m_point_light4_attenuation = XMFLOAT3(0.0f, 0.0f, 1.0f);
#pragma endregion

	// Set directional light rotation vector
	m_rotate_directional_light = XMMatrixIdentity();

	// Add models to list
	m_Models.push_back(enemy0); // 0
	m_Models.push_back(m_player); // 1
	m_Models.push_back(meleeSphere); // 2
	m_Models.push_back(spell); // 3
	m_Models.push_back(pushable_cube0); // 4
	m_Models.push_back(pushable_cube1); // 5
	m_Models.push_back(cube_trigger0); // 6
	m_Models.push_back(cube_trigger1); // 7
	m_Models.push_back(door); // 8
	m_Models.push_back(life_pickup0); // 9
	m_Models.push_back(enemy1); // 10
	m_Models.push_back(enemy2); // 11
	m_Models.push_back(life_pickup1); // 12

	return S_OK;
}

void Game::GameLoop()
{
	// Handle Input
	m_InputHandling->ReadInputStates();
	m_InputHandling->HandleInput(m_hWnd, m_player);
	CollisionCheck();

	// Update objects (including player)
	for (int i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i] && m_Models[i]->IsActive())
		{
			m_Models[i]->Update();
		}
	}

	// Render
	RenderFrame();
}
void Game::ShutdownD3D()
{
	if (m_plane)
	{
		delete m_plane;
		m_plane = nullptr;
	}
	if (m_skybox)
	{
		delete m_skybox;
		m_skybox = nullptr;
	}
	if (m_InputHandling)
	{
		m_InputHandling->ShutdownInput();

		delete m_InputHandling;
		m_InputHandling = nullptr;
	}

#pragma region Delete Models
	// Cast each model to its type before deleting to ensure delete goes through all inherited classes
	Enemy* enemy0 = static_cast<Enemy*>(m_Models[0]);
	delete enemy0;
	m_Models[0] = nullptr;

	Player* player = static_cast<Player*>(m_Models[1]);
	delete player;
	m_Models[1] = nullptr;

	MeleeSphere* melee_sphere = static_cast<MeleeSphere*>(m_Models[2]);
	delete melee_sphere;
	m_Models[2] = nullptr;

	Spell* spell = static_cast<Spell*>(m_Models[3]);
	delete spell;
	m_Models[3] = nullptr;

	Model* pushable_cube0 = static_cast<Model*>(m_Models[4]);
	delete pushable_cube0;
	m_Models[4] = nullptr;

	Model* pushable_cube1 = static_cast<Model*>(m_Models[5]);
	delete pushable_cube1;
	m_Models[5] = nullptr;

	CubeTrigger* cube_trigger0 = static_cast<CubeTrigger*>(m_Models[6]);
	delete cube_trigger0;
	m_Models[6] = nullptr;

	CubeTrigger* cube_trigger1 = static_cast<CubeTrigger*>(m_Models[7]);
	delete cube_trigger1;
	m_Models[7] = nullptr;

	Door* door = static_cast<Door*>(m_Models[8]);
	delete door;
	m_Models[8] = nullptr;

	LifePickup* life_pickup0 = static_cast<LifePickup*>(m_Models[9]);
	delete life_pickup0;
	m_Models[9] = nullptr;

	Enemy* enemy1 = static_cast<Enemy*>(m_Models[10]);
	delete enemy1;
	m_Models[10] = nullptr;

	Enemy* enemy2 = static_cast<Enemy*>(m_Models[11]);
	delete enemy2;
	m_Models[11] = nullptr;

	LifePickup* life_pickup1 = static_cast<LifePickup*>(m_Models[12]);
	delete life_pickup1;
	m_Models[12] = nullptr;
#pragma endregion
	if (m_Sprite)
	{
		delete m_Sprite;
		m_Sprite = nullptr;
	}
	if (m_2DText0)
	{
		delete m_2DText0;
		m_2DText0 = nullptr;
	}
	if (m_2DText1)
	{
		delete m_2DText1;
		m_2DText1 = nullptr;
	}
	if (m_particleGenerator)
	{
		delete m_particleGenerator;
		m_particleGenerator = nullptr;
	}

	if (m_pAlphaBlendEnable)	m_pAlphaBlendEnable->Release();
	if (m_pAlphaBlendDisable)	m_pAlphaBlendDisable->Release();
	if (m_pZBuffer)				m_pZBuffer->Release();
	if (m_pBackBufferRTView)	m_pBackBufferRTView->Release();
	if (m_pSwapChain)			m_pSwapChain->Release();
	if (m_pImmediateContext)	m_pImmediateContext->Release();
	if (m_pD3DDevice)			m_pD3DDevice->Release();
}
void Game::CollisionCheck()
{
	// Check all models against each other to see if they've collided
	for (int i = 0; i < m_Models.size(); i++)
	{
		for (int j = 0; j < m_Models.size(); j++)
		{
			// Make sure both models are active when checking collisions
			if (m_Models[i]->IsActive() && m_Models[j]->IsActive() && m_Models[i]->CheckCollision(m_Models[j]))
			{
				// If both are constant they can't enter each other
				if ((m_Models[i]->GetCollisionType() == CONSTANT && m_Models[j]->GetCollisionType() == CONSTANT) ||
					(m_Models[i]->GetCollisionType() == CONSTANT && m_Models[j]->GetCollisionType() == PUSHABLE))
				{
					// Ensure the player can't avoid collision by going sideways/backwards
					if (m_Models[i] == m_player)
					{
						// The player can move pushable items
						if (m_Models[j]->GetCollisionType() == PUSHABLE && m_player->CheckCanPush(m_Models[j]))
						{
							if (m_InputHandling->IsKeyPressed(DIK_W)) m_player->Forward(-0.02);
							if (m_InputHandling->IsKeyPressed(DIK_S)) m_player->Forward(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_D)) m_player->Strafe(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_A)) m_player->Strafe(-0.02);
							// Angle the pushable object according to the camera rotation
							float dx = (float)sin(m_player->GetCameraRotation() * (XM_PI / 180.0));
							float dz = (float)cos(m_player->GetCameraRotation() * (XM_PI / 180.0));
							float yAngle = atan2(dx, dz) * (180.0 / XM_PI);
							m_Models[j]->SetYAngle(yAngle);
							m_Models[j]->MoveForward(1);
						}
						// Otherwise the player can trigger an objects OnCollision function
						else if (m_Models[j]->GetCollisionType() != PUSHABLE)
						{
							if (m_InputHandling->IsKeyPressed(DIK_W)) m_player->Forward(-0.02);
							if (m_InputHandling->IsKeyPressed(DIK_S)) m_player->Forward(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_D)) m_player->Strafe(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_A)) m_player->Strafe(-0.02);
							m_Models[j]->OnCollision(m_Models[i]);
						}
						// Make sure we can't go backwards or sideways through a pusable but can go forwards
						else
						{
							if (m_InputHandling->IsKeyPressed(DIK_S)) m_player->Forward(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_D)) m_player->Strafe(0.02);
							if (m_InputHandling->IsKeyPressed(DIK_A)) m_player->Strafe(-0.02);
						}
					}
					else
					{
						// If an enemy has hit the player they should damage them and the player should be pushed back
						Enemy* enemy = static_cast<Enemy*>(m_Models[i]);
						if (enemy && m_Models[j] == m_player)
						{
							m_player->ChangeHealth(-20);
							m_player->Forward(-5);
						}
						// If the enemy hits something else it should turn a random number of degrees and jump back
						else
						{
							int rand = std::rand() % 360;
							m_Models[i]->SetYAngle(m_Models[i]->GetYAngle() + rand);
							m_Models[i]->MoveForward(-10);
						}
					}
				}
				// If the player collides with a pick up they pick it up
				else if (m_Models[i] == m_player && m_Models[j]->GetCollisionType() == PICKUP)
				{
					// TODO: Increase player health and disable object
					m_Models[j]->OnCollision(m_player);
				}
				// If a constant object is in a trigger then it's triggered
				else if (m_Models[i]->GetCollisionType() == CONSTANT && m_Models[j]->GetCollisionType() == TRIGGER)
				{
					// Ensure player melee attack doesn't affect player, likewise for the spell
					bool player_and_their_melee_sphere = (m_Models[i] == m_player->GetMeleeSphere() && m_Models[j] == m_player) || (m_Models[j] == m_player->GetMeleeSphere() && m_Models[i] == m_player);
					bool player_and_their_spell = (m_Models[i] == m_player->GetSpell() && m_Models[j] == m_player) || (m_Models[j] == m_player->GetSpell() && m_Models[i] == m_player);
					if (!player_and_their_melee_sphere && !player_and_their_spell)
					{
						m_Models[j]->OnCollision(m_Models[i]);
					}
				}
				// If a pushable object is pushed into a constant object or another pushable it should jump back
				else if ((m_Models[i]->GetCollisionType() == PUSHABLE && m_Models[j]->GetCollisionType() == CONSTANT) ||
						(m_Models[i]->GetCollisionType() == PUSHABLE && m_Models[j]->GetCollisionType() == PUSHABLE))
				{
					m_Models[j]->MoveForward(-2);
				}
				// Pushable objects trigger triggers
				else if (m_Models[i]->GetCollisionType() == TRIGGER && m_Models[j]->GetCollisionType() == PUSHABLE)
				{
					m_Models[i]->OnCollision(m_Models[j]);
				}
			}
		}
	}
}
void Game::RenderFrame(void)
{
	// Clear the view
	float rgba_clear_colour[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);
	m_pImmediateContext->ClearDepthStencilView(m_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// World View Projection Matrix
	XMMATRIX projection, world, view;
	world = XMMatrixTranslation(0, 0, 0);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 640.0 / 480.0, 1.0, 100.0);
	view = m_player->GetViewMatrix();

	// Add point lights
	m_point_light0_position = XMVectorSet(m_Models[0]->GetX(), -4.0f, m_Models[0]->GetZ(), 0.0f);
	m_point_light1_position = XMVectorSet(m_Models[6]->GetX(), -4.0f, m_Models[6]->GetZ(), 0.0f);
	m_point_light2_position = XMVectorSet(m_Models[7]->GetX(), -4.0f, m_Models[7]->GetZ(), 0.0f);
	m_point_light3_position = XMVectorSet(m_Models[10]->GetX(), -4.0f, m_Models[10]->GetZ(), 0.0f);
	m_point_light4_position = XMVectorSet(m_Models[11]->GetX(), -4.0f, m_Models[11]->GetZ(), 0.0f);

	// Show Skybox
	m_skybox->RenderSkybox(&view, &projection, m_player->GetX(), m_player->GetY(), m_player->GetZ());

	// Show plane
	m_plane->AddAmbientLight(m_ambient_light_colour);
	m_plane->AddDirectionalLight(m_directional_light_shines_from, m_directional_light_colour, m_rotate_directional_light);

	// Modify point light colours according to states of enemies/cube triggers
	if (!m_Models[0]->IsActive())
	{
		m_point_light0_colour = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
	if (!m_Models[10]->IsActive())
	{
		m_point_light3_colour = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
	if (!m_Models[11]->IsActive())
	{
		m_point_light4_colour = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CubeTrigger* cube_trigger0 = static_cast<CubeTrigger*>(m_Models[6]);
	if (cube_trigger0 && cube_trigger0->IsTriggered())
	{
		m_point_light1_colour = XMVectorSet(0.0f, 0.01f, 0.0f, 0.0f);
	}
	else
	{
		m_point_light1_colour = XMVectorSet(0.01f, 0.0f, 0.0f, 0.0f);
	}
	CubeTrigger* cube_trigger1 = static_cast<CubeTrigger*>(m_Models[7]);
	if (cube_trigger1 && cube_trigger1->IsTriggered())
	{
		m_point_light2_colour = XMVectorSet(0.0f, 0.01f, 0.0f, 0.0f);
	}
	else
	{
		m_point_light2_colour = XMVectorSet(0.01f, 0.0f, 0.0f, 0.0f);
	}

	// POINT LIGHTS
	m_plane->ClearPointLights();
	m_plane->AddPointLight(m_point_light0_position, m_point_light0_colour, m_point_light0_attenuation);
	m_plane->AddPointLight(m_point_light1_position, m_point_light1_colour, m_point_light1_attenuation);
	m_plane->AddPointLight(m_point_light2_position, m_point_light2_colour, m_point_light2_attenuation);
	m_plane->AddPointLight(m_point_light3_position, m_point_light3_colour, m_point_light3_attenuation);
	m_plane->AddPointLight(m_point_light4_position, m_point_light4_colour, m_point_light4_attenuation);
	m_plane->RenderPlane(&view, &projection);

	// Add lighting to and draw all models
	for (int i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i]->IsActive())
		{
			m_Models[i]->AddAmbientLight(m_ambient_light_colour);
			m_Models[i]->AddDirectionalLight(m_directional_light_shines_from, m_directional_light_colour, m_rotate_directional_light);
			m_Models[i]->ClearPointLights();
			m_Models[i]->AddPointLight(m_point_light0_position, m_point_light0_colour, m_point_light0_attenuation);
			m_Models[i]->AddPointLight(m_point_light1_position, m_point_light1_colour, m_point_light1_attenuation);
			m_Models[i]->AddPointLight(m_point_light2_position, m_point_light2_colour, m_point_light2_attenuation);
			m_Models[i]->AddPointLight(m_point_light3_position, m_point_light3_colour, m_point_light3_attenuation);
			m_Models[i]->AddPointLight(m_point_light4_position, m_point_light4_colour, m_point_light4_attenuation);
			m_Models[i]->Draw(&view, &projection);
		}
	}

	// Show particles
	if (m_player->GetSpell()->IsActive())
	{
		m_particleGenerator->SetX(m_player->GetSpell()->GetX());
		m_particleGenerator->SetY(m_player->GetSpell()->GetY());
		m_particleGenerator->SetZ(m_player->GetSpell()->GetZ());
		m_particleGenerator->Draw(&view, &projection, m_player->GetX(), m_player->GetY(), m_player->GetZ());
	}

	// Show UI Sprites
	m_Sprite->RenderSprites();
	
	// Attacking HUD Notif
	// Only show spell or attack so they don't overlap
	if (m_player->IsAttacking())
	{
		m_Sprite->AddBox(6, -1.0, -0.9, 0.1);
		m_2DText0->RenderText();
		m_2DText0->AddText("Attack!", -1.0, -0.9, 0.08);
	}
	else if (m_player->GetSpell()->IsActive())
	{
		m_Sprite->AddBox(5, -1.0, -0.9, 0.1);
		m_2DText0->RenderText();
		m_2DText0->AddText("Spell!", -1.0, -0.9, 0.08);
	}

	// Health HUD
	if (m_player->IsActive())
	{
		m_Sprite->AddBox(12, -1.0, 1.0, 0.1);
		m_2DText1->RenderText();
		m_2DText1->AddText("Health.." + std::to_string(m_player->GetHealth()) + "/" + std::to_string(m_player->GetMaxHealth()), -1.0, 1.0, 0.08);
		m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisable, 0, 0xffffffff);
	}
	// If the player died they lose
	else if (m_player->GetHealth() <= 0)
	{
		m_Sprite->AddBox(8, -1.0, 1.0, 0.1);
		m_2DText1->RenderText();
		m_2DText1->AddText("Game Over", -1.0, 1.0, 0.08);
		m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisable, 0, 0xffffffff);
	}
	// If the player is disabled but their health is above 0 they have won
	else
	{
		m_Sprite->AddBox(11, -1.0, 1.0, 0.1);
		m_2DText1->RenderText();
		m_2DText1->AddText("Game Complete", -1.0, 1.0, 0.08);
		m_pImmediateContext->OMSetBlendState(m_pAlphaBlendDisable, 0, 0xffffffff);
	}

	m_pSwapChain->Present(0, 0);
}
void Game::ResizeWindow(UINT new_size[2])
{
	m_pImmediateContext->OMSetRenderTargets(0, 0, 0);

	m_pBackBufferRTView->Release();
	m_pZBuffer->Release();

	// Render Texture
	HRESULT hr;
	hr = m_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11Texture2D* pBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);

	hr = m_pD3DDevice->CreateRenderTargetView(pBuffer, NULL, &m_pBackBufferRTView);
	pBuffer->Release();

	// Z Buffer
	DXGI_SWAP_CHAIN_DESC sd;
	m_pSwapChain->GetDesc(&sd);

	D3D11_TEXTURE2D_DESC tex2dDesc;
	std::ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = new_size[0];
	tex2dDesc.Height = new_size[1];
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = m_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	std::ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &m_pZBuffer);
	pZBufferTexture->Release();

	// Final Setup
	m_pImmediateContext->OMSetRenderTargets(1, &m_pBackBufferRTView, m_pZBuffer);

	D3D11_VIEWPORT vp;
	vp.Width = new_size[0];
	vp.Height = new_size[1];
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &vp);
}