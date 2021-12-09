TextureCube texture0;
TextureCube texture1;
SamplerState sampler0;

cbuffer CBuffer0 // 224 bytes
{
	matrix WVPMatrix;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float4 point_light_position;
	float4 point_light_colour;
	float3 point_light_attenuation;
	float packing_1;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float3 texcoord : TEXCOORD;
};

VOut ReflectiveModelVS(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);

	// Reflectiveness
	float3 wvpos = mul(WVPMatrix, position);
	float3 wvnormal = mul(WVPMatrix, normal);
	wvnormal = normalize(normal);
	float3 eyer = -normalize(wvpos);
	output.texcoord = 2.0 * dot(eyer, normal) * wvnormal - eyer;

	return output;
}

float4 ReflectiveModelPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord) * texture1.Sample(sampler0, texcoord);
}