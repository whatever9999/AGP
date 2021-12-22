Texture2D texture0;
SamplerState sampler0;

cbuffer CBuffer0 // 64 bytes
{
	matrix WVPMatrix;
};

cbuffer PCBuffer0 // 96 bytes
{
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
	float4 pixel_position : SP_POSITION;
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
	float3 normal	: NORMAL;
};

VOut PlaneVS(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal: NORMAL)
{
	VOut output;

	output.pixel_position = position;
	output.position = mul(WVPMatrix, position);
	output.color = color;
	output.texcoord = texcoord;
	output.normal = normal;

	return output;
}

float4 PlanePS(float4 pixel_position : SP_POSITION, float4 position : SV_POSITION, float4 color : COLOR0, float2 texcoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET
{
	// Directional Light
	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);

	// Point Light
	float4 light_vector = point_light_position - pixel_position;
	float point_amount = dot(normalize(light_vector), normal);
	point_amount = saturate(point_amount);
	// Attenuation
	float distanceX = point_light_position.x - pixel_position.x;
	float distanceZ = point_light_position.z - pixel_position.z;
	distanceX *= distanceX;
	distanceZ *= distanceZ;
	float distance = sqrt(distanceX  + distanceZ);

	float denominator = 0.0f;
	if (point_light_attenuation[0]) denominator += point_light_attenuation[0];
	if (point_light_attenuation[1]) denominator += point_light_attenuation[1] * distance;
	if (point_light_attenuation[2]) denominator += point_light_attenuation[2] * (distance * distance);

	float4 attenuated_point_light;
	if (!denominator)
	{
		attenuated_point_light = point_light_colour;
	}
	else
	{
		attenuated_point_light  = point_light_colour / denominator;
	}

	// Set colour with lighting taken into account
	float4 final_colour = (ambient_light_colour + (directional_light_colour * diffuse_amount) + (attenuated_point_light * point_amount));

	return final_colour * texture0.Sample(sampler0, texcoord);
}
