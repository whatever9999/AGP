Texture2D texture0;
Texture2D texture1;
SamplerState sampler0;

cbuffer CBuffer0 // 128 bytes
{
	matrix WVPMatrix;
	matrix world;
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
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal	: NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position	= mul(WVPMatrix, position);
	output.texcoord = texcoord;
	output.normal = mul(world, float4(normal, 1));

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET
{
	// Directional Light
	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);

	// Point Light
	float4 lightvector = point_light_position - position;
	float point_amount = dot(normal, normalize(lightvector));
	point_amount = saturate(point_amount);
	// Attenuation
	float magnitude = length(lightvector);
	float4 attenuated_point_light = point_light_colour / (magnitude * magnitude);

	// Set colour with lighting taken into account
	float4 color = float4(ambient_light_colour.xyz, 1) + (float4(directional_light_colour.xyz, 1) * diffuse_amount) + (float4(attenuated_point_light.xyz, 1) * point_amount);
	return color * texture0.Sample(sampler0, texcoord) * texture1.Sample(sampler0, texcoord);
}