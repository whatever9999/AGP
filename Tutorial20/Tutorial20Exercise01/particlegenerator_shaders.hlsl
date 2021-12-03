TextureCube texture0;
TextureCube texture1;
SamplerState sampler0;

cbuffer CBuffer0 // 80 bytes
{
	matrix WVPMatrix;
	float4 color;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

VOut ParticleGeneratorVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);
	output.color = color;
	output.texcoord = position.xy;

	return output;
}

float4 ParticleGeneratorPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	float distaq = texcoord.x * texcoord.x + texcoord.y * texcoord.y;
	clip(1.0f - distaq);
	return color;
}