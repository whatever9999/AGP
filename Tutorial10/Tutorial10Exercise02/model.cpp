#include "model.h"

struct MODEL_CONSTANT_BUFFER // 160 bytes
{
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	XMVECTOR point_light_position;
	XMVECTOR point_light_colour;
	XMFLOAT3 point_light_attenuation;
	float packing_1;
};


HRESULT Model::Setup()
{
	// Shaders
	HRESULT hr = S_OK;
	ID3DBlob* VS, * PS, * error;

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
}

HRESULT Model::LoadObjModel(char* filename)
{
	HRESULT hr = S_OK;

	m_pObject = new ObjFileModel(filename, m_D3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "FILE NOT LOADED")
	{
		return S_FALSE;
	}

	hr = Setup();

	return hr;
}

void Model::AddTextures(char* texture0_filename, char* texture1_filename)
{
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
	D3DX11CreateShaderResourceViewFromFile(m_D3DDevice, texture0_filename, NULL, NULL, &m_pTexture0, NULL);
	D3DX11CreateShaderResourceViewFromFile(m_D3DDevice, texture1_filename, NULL, NULL, &m_pTexture1, NULL);
}

void Model::AddDirectionalLight(XMVECTOR directional_light_shines_from, XMVECTOR directional_light_colour, XMMATRIX rotate_directional_light)
{
	m_directional_light_shines_from = directional_light_shines_from;
	m_directional_light_colour = directional_light_colour;
	m_rotate_directional_light = rotate_directional_light;
}
void Model::AddAmbientLight(XMVECTOR ambient_light_colour)
{
	m_ambient_light_colour = ambient_light_colour;
}
void Model::AddPointLight(XMVECTOR point_light_position, XMVECTOR point_light_colour, XMFLOAT3 point_light_attenuation)
{
	m_point_light_position = point_light_position;
	m_point_light_colour = point_light_colour;
	m_point_light_attenuation = point_light_attenuation;
}

void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	// Lighting colours
	model_cb_values.point_light_colour = m_point_light_colour;
	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.point_light_attenuation = m_point_light_attenuation;

	// Lighting positions
	XMMATRIX transpose = XMMatrixTranspose(world);
	XMVECTOR determinant;
	XMMATRIX inverse = XMMatrixInverse(&determinant, world);
	model_cb_values.directional_light_vector = XMVector3Transform(XMVector3Transform(m_directional_light_shines_from, m_rotate_directional_light), transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);
	model_cb_values.point_light_position = XMVector3Transform(m_point_light_position, inverse);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

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

	m_pObject->Draw();
}