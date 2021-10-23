cbuffer CBuffer0
{
	float3 pos_change;
	float scale;
	int choice;
	float4 colour_1;
	float4 colour_2;
	float4 colour_3;
	float4 colour_4;
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

	if (choice == 1)
	{
		output.color = colour_1;
	}
	else if (choice == 2)
	{
		output.color = colour_2;
	}
	else if (choice == 3)
	{
		output.color = colour_3;
	}
	else if (choice == 4)
	{
		output.color = colour_4;
	}

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}