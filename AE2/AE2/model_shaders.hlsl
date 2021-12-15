Texture2D texture0;
Texture2D texture1;
SamplerState sampler0;

cbuffer CBuffer0 // 64 bytes
{
	matrix WVPMatrix;
};

cbuffer CPBuffer0 // 96 bytes
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

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = position;
	output.position	= mul(WVPMatrix, position);
	output.texcoord = texcoord;
	output.normal = normal;

	return output;
}

float4 ModelPS(float4 pixel_position : SP_POSITION, float4 position : SV_POSITION, float4 color : COLOR0, float2 texcoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET
{
	// Directional Light
	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);

	// Point Light
	float4 lightvector = point_light_position - pixel_position;
	float point_amount = dot(normalize(lightvector), normal);
	point_amount = saturate(point_amount);
	// Attenuation
	float distance = length(lightvector);
	float4 attenuated_point_light = point_light_colour / (distance * distance);

	// Set colour with lighting taken into account
	float4 final_colour = (ambient_light_colour + (directional_light_colour * diffuse_amount) + (point_light_colour * point_amount));

	return final_colour * texture0.Sample(sampler0, texcoord) * texture1.Sample(sampler0, texcoord);
}