TextureCube texture0;
SamplerState sampler0;

cbuffer CBuffer0 // 64 bytes
{
	matrix WVPMatrix;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float3 texcoord : TEXCOORD;
};

VOut SkyboxVS(float4 position : POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD, float3 normal: NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);
	output.color = color;
	output.texcoord = position.xyz;

	return output;
}

float4 SkyboxPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord);
}