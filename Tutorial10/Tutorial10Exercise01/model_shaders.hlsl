Texture2D texture0;
Texture2D texture1;
SamplerState sampler0;

cbuffer CBuffer0 // 128 bytes
{
	matrix WVPMatrix;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal: NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);
	output.texcoord = texcoord;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord) * texture1.Sample(sampler0, texcoord);
}