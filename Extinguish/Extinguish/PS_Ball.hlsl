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
	float4 finalColor;
	float3 diffuseColor, dirColor, pointColor, spotColor;
	
	//initialize point and spot colors
	pointColor = float3(0, 0, 0);
	spotColor = float3(0, 0, 0);
	
	//get texture color with uvs help
	diffuseColor = (float3)baseTexture.Sample(filter, input.uv);
	
	//calculate dircolor
	float lightRatio;
	float3 black = { 0.4f, 0.4f, 0.4f };
	
	lightRatio = saturate(dot((float3)normalize(dirLightNorm), normalize(input.normal)));
	dirColor = (lightRatio + (float3)ambientLight) * (float3)dirLightColor * black;
	
	//calculate pointlight colors
	for (int i = 0; i < NUMOFPOINTLIGHTS; ++i)
	{
		float pointRatio;
		float4 pointDir;
		float pointAttenuation;
	
		pointDir = pLights[i].pointLightPosition - input.worldPosition;
		pointAttenuation = 1 - saturate(length(pointDir) / pLights[i].lightRadius.x);
		pointRatio = saturate(dot((float3)normalize(pointDir), normalize(input.normal)));
	
		pointColor += (float3)pLights[i].pointLightColor * saturate(pointRatio * pointAttenuation) * black;
	}


	//calculate final color
	finalColor = float4(saturate((dirColor + pointColor + spotColor) * diffuseColor), 1.0f);

	float4 toBall = input.worldPosition - camPos;
	float doting = dot(normalize(toBall.xyz), input.normal);
	float tbl = length(toBall);
	if (tbl > 5)
	{
		if (doting >= tbl * -0.01f && doting <= tbl * 0.01f)
		{
			finalColor.r = 0.50f;
			finalColor.g = 1;
			finalColor.b = 0;
			finalColor.a = 1;
		}
	}

	return finalColor;
}