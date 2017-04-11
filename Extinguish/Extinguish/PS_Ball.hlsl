#define NUMOFPOINTLIGHTS 4

struct PS_BasicInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 worldPosition : POSITION0;
};

cbuffer DirectionalLightCB : register(b0)
{
	float4 dirLightNorm;
	float4 dirLightColor;
	float4 ambientLight;
};

struct PointLight
{
	float4 pointLightPosition;
	float4 pointLightColor;
	float4 lightRadius; //treat as float
};

cbuffer PointLightCB : register(b1)
{
	PointLight pLights[NUMOFPOINTLIGHTS];
};

cbuffer CamPos : register(b2)
{
	float4 camPos;
};

texture2D baseTexture : register(t0);

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{
	return float4(0,0.85f,0,1);
}