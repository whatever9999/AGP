cbuffer CBuffer0
{
	float3 pos_change;
	float scale;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position	= position;
	output.position.x += pos_change.x;
	output.position.y += pos_change.y;
	output.position.xy *= scale;

	output.color	= color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}