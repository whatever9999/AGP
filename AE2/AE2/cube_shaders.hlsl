Texture2D texture0;
SamplerState sampler0;

cbuffer CBuffer0 // 64 bytes
{
	matrix WVPMatrix;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

VOut CubeVS(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal: NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);
	output.color = color;
	output.texcoord = texcoord;

	return output;
}

float4 CubePS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}