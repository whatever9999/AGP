struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position	= position;
	output.color = color;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	float4 new_colour = color;

	for (int i = 0; i < 4; i++)
	{
		new_colour[i] = position[i] + new_colour[i];
		if (new_colour[i] > 1.0f)
		{
			new_colour[i] -= 1.0f;
		}
	}

	color = new_colour;

	return color;
}