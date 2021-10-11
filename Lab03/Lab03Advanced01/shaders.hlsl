struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position	= position;

	for (int i = 0; i < 4; i++)
	{
		output.color[i] = position[i] + color[i];
		if (output.color[i] > 1.0f)
		{
			output.color[i] -= 1.0f;
		}
	}

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}