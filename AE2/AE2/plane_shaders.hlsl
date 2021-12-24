Texture2D texture0;
SamplerState sampler0;

cbuffer CBuffer0 // 64 bytes
{
	matrix WVPMatrix;
};

cbuffer PCBuffer0 // 288 bytes
{
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;

	// POINT LIGHTS
	float4 point_light0_position;
	float4 point_light0_colour;
	float3 point_light0_attenuation;
	float packing_0;

	float4 point_light1_position;
	float4 point_light1_colour;
	float3 point_light1_attenuation;
	float packing_1;

	float4 point_light2_position;
	float4 point_light2_colour;
	float3 point_light2_attenuation;
	float packing_2;

	float4 point_light3_position;
	float4 point_light3_colour;
	float3 point_light3_attenuation;
	float packing_3;

	float4 point_light4_position;
	float4 point_light4_colour;
	float3 point_light4_attenuation;
	float packing_4;
};

struct VOut
{
	float4 pixel_position : SP_POSITION;
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
	float3 normal	: NORMAL;
};

float4 GetAttenuatedPointLight(float3 attenuation, float4 colour, float4 position, float4 pixel_position, float3 normal)
{
	// Point Light
	float4 light_vector = position - pixel_position;
	float point_amount = dot(normalize(light_vector), normal);
	point_amount = saturate(point_amount);
	// Attenuation
	float distanceX = position.x - pixel_position.x;
	float distanceZ = position.z - pixel_position.z;
	distanceX *= distanceX;
	distanceZ *= distanceZ;
	float distance = sqrt(distanceX + distanceZ);

	float denominator = 0.0f;
	if (attenuation[0]) denominator += attenuation[0];
	if (attenuation[1]) denominator += attenuation[1] * distance;
	if (attenuation[2]) denominator += attenuation[2] * (distance * distance);

	float4 attenuated_point_light;
	if (!denominator)
	{
		attenuated_point_light = colour;
	}
	else
	{
		attenuated_point_light = colour / denominator;
	}

	return attenuated_point_light;
}


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

	// Point Lights
	float4 attenuated_point_light0 = GetAttenuatedPointLight(point_light0_attenuation, point_light0_colour, point_light0_position, pixel_position, normal);
	float4 attenuated_point_light1 = GetAttenuatedPointLight(point_light1_attenuation, point_light1_colour, point_light1_position, pixel_position, normal);
	float4 attenuated_point_light2 = GetAttenuatedPointLight(point_light2_attenuation, point_light2_colour, point_light2_position, pixel_position, normal);
	float4 attenuated_point_light3 = GetAttenuatedPointLight(point_light3_attenuation, point_light3_colour, point_light3_position, pixel_position, normal);
	float4 attenuated_point_light4 = GetAttenuatedPointLight(point_light4_attenuation, point_light4_colour, point_light4_position, pixel_position, normal);

	// Set colour with lighting taken into account
	float4 final_colour = (ambient_light_colour + (directional_light_colour * diffuse_amount) + attenuated_point_light0 + attenuated_point_light1 + attenuated_point_light2 + attenuated_point_light3 + attenuated_point_light4);

	return final_colour * texture0.Sample(sampler0, texcoord);
}