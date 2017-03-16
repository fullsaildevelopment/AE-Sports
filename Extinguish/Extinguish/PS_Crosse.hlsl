#define NUMOFPOINTLIGHTS 4

struct PS_BasicInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : TEXCOORD;
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
	//float4 pointLightPosition[NUMOFPOINTLIGHTS];
	//float4 pointLightColor[NUMOFPOINTLIGHTS];
	//float4 lightRadius[NUMOFPOINTLIGHTS]; //treat as float
};

cbuffer CameraPosition : register(b2)
{
	float4 cameraposW;
}

cbuffer TeamColorCB : register(b3)
{
	float4 TeamColorB;
}


texture2D baseTexture : register(t0);
texture2D normalMap : register(t1);
texture2D specularMap : register(t2);
texture2D teamcolorMap : register(t3);

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{
	float4 finalColor = {0,0,0,0.5f};
	float4 tcm = teamcolorMap.Sample(filter, input.uv.xy);
	float4 netc = baseTexture.Sample(filter, input.uv.xy);

	if (tcm.r >= 0.96f && tcm.g >= 0.96f && tcm.b >= 0.96f)
	{
		finalColor.r = TeamColorB.r;
		finalColor.g = TeamColorB.g;
		finalColor.b = TeamColorB.b;
	}

	if (netc.r >= 0.2f && TeamColorB.a > 0)
	{
		finalColor.g = 1.0f;
	}
	else if (netc.r >= 0.5f) discard;
	
	return finalColor;
}