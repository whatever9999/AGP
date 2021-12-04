#include "particlegenerator.h"

struct PARTICLEGENERATOR_CONSTANT_BUFFER // 80 bytes
{
	XMMATRIX WorldViewProjection;
	XMFLOAT4 Colour;
};

float ParticleGenerator::RandomZeroToOne()
{
	float value = ((float)rand() / (RAND_MAX)) + 1;
	return value;
}
float ParticleGenerator::RandomNegOneToPosOne()
{
	float value = ((float(rand()) / float(RAND_MAX)) * (1 - (-1))) + (-1);
	return value;
}

void ParticleGenerator::Update()
{
	if (m_untilParticle <= 0.0f)
	{
		if (m_isActive)
		{
			std::list<Particle*>::iterator it = m_free.begin();
			if (m_free.size() != NULL)
			{
				m_untilParticle = 3.0f;
				(*it)->color = XMFLOAT4(RandomZeroToOne(), RandomZeroToOne(), RandomZeroToOne(), 1.0f);
				(*it)->gravity = 4.5f;
				(*it)->position = XMFLOAT3(0.0f, -6.0f, 12.0f);
				(*it)->velocity = XMFLOAT3(RandomNegOneToPosOne(), 4.50f, RandomNegOneToPosOne());
				(*it)->age = 0.0f;

				m_active.push_back(*it);
				m_free.pop_front();
			}
		}
		else
		{
			m_untilParticle = 0.001f;
		}
	}
}

HRESULT ParticleGenerator::Setup()
{
#pragma region Vertices
	XMFLOAT3 vertices[6] =
	{
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.0f), 
		XMFLOAT3(1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
	};
#pragma endregion

	// Shaders
	HRESULT hr = S_OK;
	ID3DBlob* VS, * PS, * error;

	hr = D3DX11CompileFromFile("particlegenerator_shaders.hlsl", 0, 0, "ParticleGeneratorVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("particlegenerator_shaders.hlsl", 0, 0, "ParticleGeneratorPS" , "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = m_D3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_D3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
	{
		return hr;
	}

	// Input Layout
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = m_D3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = m_D3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	// Constant Buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 160;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_D3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	// Setup Sampler
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	m_D3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);

	// Rasteriser
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	hr = m_D3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterSolid);
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	hr = m_D3DDevice->CreateRasterizerState(&rasterizer_desc, &m_pRasterParticle);

	// Initialise particles
	for (int i = 0; i < 100; i++)
	{
		Particle* particle = new Particle();
		m_free.push_back(particle);
	}

	return S_OK;
}

void ParticleGenerator::Draw(XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z)
{
	// Update Time
	float timeNow = float(timeGetTime() / 1000.0f);
	float deltaTime = timeNow - m_timePrevious;
	m_timePrevious = timeNow;
	m_untilParticle -= deltaTime;

	// Update Particles
	Update();

	// Draw Active Particles
	if (m_active.size() != NULL)
	{
		std::list<Particle*>::iterator it = m_active.begin();
		while (it != m_active.end())
		{
			// Update Particle Age
			(*it)->age += deltaTime;
			(*it)->velocity.y -= (*it)->gravity * (deltaTime);
			(*it)->position.x += (*it)->velocity.x * (deltaTime);
			(*it)->position.y += (*it)->velocity.y * (deltaTime);
			(*it)->position.z += (*it)->velocity.z * (deltaTime);

			DrawOne((*it), view, projection, camera_x, camera_y, camera_z);

			// Remove old particles
			if ((*it)->age >= m_age)
			{
				it++;
				m_active.front()->age = m_age;
				m_free.push_back(m_active.front());		
				m_active.pop_front();		
			}
			else it++;
		}
	}
}
void ParticleGenerator::DrawOne(Particle* one, XMMATRIX* view, XMMATRIX* projection, float camera_x, float camera_y, float camera_z)
{
	XMMATRIX world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationY(XM_PI);
	world *= XMMatrixTranslation(one->position.x, one->position.y, one->position.z);

	PARTICLEGENERATOR_CONSTANT_BUFFER particlegenerator_cb_values;
	particlegenerator_cb_values.WorldViewProjection = world * (*view) * (*projection);

	// Colour
	particlegenerator_cb_values.Colour = one->color;

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &particlegenerator_cb_values, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	if (m_pTexture0 && m_pTexture1)
	{
		m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
		m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
		m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pImmediateContext->PSSetShaderResources(1, 1, &m_pTexture1);
	}

	// Draw
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->Draw(numverts, 0);
}