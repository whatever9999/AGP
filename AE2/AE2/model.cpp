#include "model.h"

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

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, m_pixel_shader, "ps_4_0", 0, 0, 0, &PS, &error, 0);

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

	return S_OK;
}

void Model::CalculateModelCentrePoint()
{
	// Get the min/max x/y/z values from this model's vertices
	float min_x = m_pObject->vertices[0].Pos.x, max_x = m_pObject->vertices[0].Pos.x;
	float min_y = m_pObject->vertices[0].Pos.y, max_y = m_pObject->vertices[0].Pos.y;
	float min_z = m_pObject->vertices[0].Pos.z, max_z = m_pObject->vertices[0].Pos.z;
	for (int i = 1; i < m_pObject->numverts; i++)
	{
		float current_x = m_pObject->vertices[i].Pos.x;
		if (current_x < min_x) min_x = current_x;
		if (current_x > max_x) max_x = current_x;

		float current_y = m_pObject->vertices[i].Pos.y;
		if (current_y < min_y) min_y = current_y;
		if (current_y > max_y) max_y = current_y;

		float current_z = m_pObject->vertices[i].Pos.z;
		if (current_z < min_z) min_z = current_z;
		if (current_z > max_z) max_z = current_z;
	}

	// Set bounding sphere centre
	m_bounding_sphere_centre_x = (max_x + min_x) / 2;
	m_bounding_sphere_centre_y = (max_y + min_y) / 2;
	m_bounding_sphere_centre_z = (max_z + min_z) / 2;
}
void Model::CalculateModelBoundingSphereRadius()
{
	// Get the min/max x/y/z values from this model's vertices
	float max_distance_squared = 0;
	for (int i = 0; i < m_pObject->numverts; i++)
	{
		float current_x = m_pObject->vertices[i].Pos.x;
		float current_y = m_pObject->vertices[i].Pos.y;
		float current_z = m_pObject->vertices[i].Pos.z;

		// Calculate the distance (squared) between this vertex and the bounding sphere centre
		float current_distance =	pow(current_x - m_bounding_sphere_centre_x, 2) + 
									pow(current_y - m_bounding_sphere_centre_y, 2) + 
									pow(current_z - m_bounding_sphere_centre_z, 2);

		if (current_distance > max_distance_squared) max_distance_squared = current_distance;
	}

	// Set bounding sphere centre
	m_bounding_sphere_radius = sqrt(max_distance_squared);
}


void Model::LookAt_XZ(float x, float z)
{
	float dx = x - m_x;
	float dz = z - m_z;

	m_yAngle = atan2(dx, dz) * (180.0 / XM_PI);
}
void Model::MoveForward(float multiplier)
{
	m_x += sin(m_yAngle * (XM_PI / 180.0)) * m_speed * multiplier;
	m_z += cos(m_yAngle * (XM_PI / 180.0)) * m_speed * multiplier;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationX(XMConvertToRadians(m_xAngle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yAngle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zAngle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	XMVECTOR offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);
	offset = XMVector3Transform(offset, world);

	return offset;
}
bool Model::CheckCollision(Model* other_model)
{
	if (other_model == this) return false;

	float x1 = XMVectorGetX(GetBoundingSphereWorldSpacePosition());
	float y1 = XMVectorGetY(GetBoundingSphereWorldSpacePosition());
	float z1 = XMVectorGetZ(GetBoundingSphereWorldSpacePosition());

	float x2 = XMVectorGetX(other_model->GetBoundingSphereWorldSpacePosition());
	float y2 = XMVectorGetY(other_model->GetBoundingSphereWorldSpacePosition());
	float z2 = XMVectorGetZ(other_model->GetBoundingSphereWorldSpacePosition());

	float distance_between_models = pow(x1 - x2, 2) +
									pow(y1 - y2, 2) +
									pow(z1 - z2, 2);

	if (distance_between_models < pow(GetBoundingSphereRadius() + other_model->GetBoundingSphereRadius(), 2)) return true;
	else return false;
}

HRESULT Model::LoadObjModel(char* filename, char* pixel_shader, char* vertex_shader)
{
	m_pixel_shader = pixel_shader;
	m_vertex_shader = vertex_shader;

	HRESULT hr = S_OK;

	m_pObject = new ObjFileModel(filename, m_D3DDevice, m_pImmediateContext);
	if (m_pObject->filename == "FILE NOT LOADED")
	{
		return S_FALSE;
	}

	hr = Setup();

	CalculateModelCentrePoint();
	CalculateModelBoundingSphereRadius();

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