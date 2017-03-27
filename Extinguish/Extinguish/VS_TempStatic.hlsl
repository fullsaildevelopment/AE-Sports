// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float3 uv : TEXCOORD;
	float3 binormal : BINORMAL;
	float4 tangent : TANGENT;
};

// Per-pixel color data passed through the pixel shader.
struct PS_BasicInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : TEXCOORD;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
	float4 worldPosition : POSITION0;
};

// Simple shader to do vertex processing on the GPU.
PS_BasicInput main(VertexShaderInput input)
{
	PS_BasicInput output;
	float4 pos = float4(input.pos, 1.0f);
	float4 norm = float4(input.normal, 0.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	output.worldPosition = pos;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass the color through without modification.
	output.uv = input.uv;

	norm = mul(norm, model);
	output.normal = (float3)norm;

	output.tangent = mul(float4(input.tangent.xyz, 0), model).xyz;
	output.tangent = normalize(output.tangent);

	output.binormal = cross(output.normal, output.tangent) * input.tangent.w;
	output.binormal = normalize(output.binormal);

	return output;
}