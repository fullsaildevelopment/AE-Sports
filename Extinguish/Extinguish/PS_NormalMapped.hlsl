#define NUMOFPOINTLIGHTS 4

struct PS_BasicInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : TEXCOORD;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
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
texture2D emissiveMap : register(t3);

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{

	float4 diffuseColor = baseTexture.Sample(filter, input.uv.xy);
	float4 bumpMap = normalMap.Sample(filter, input.uv.xy);
	float4 specMap = specularMap.Sample(filter, input.uv.xy);
	float4 emisMap = emissiveMap.Sample(filter, input.uv.xy);
	float3 Normal = input.normal;
	if (bumpMap.a != 0.0f)
	{
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		float3x3 TBNMatrix = float3x3(input.tangent, input.binormal, input.normal);
		Normal = normalize(mul(bumpMap.xyz, TBNMatrix));
	}

	float black = 0.5f;

	//create new normals based on normals

	float3 ViewVector = normalize(cameraposW.xyz - input.worldPosition.xyz);

	//initialize point and spot colors
	float4 directionalColor = float4(0, 0, 0, 0);
	float4 directionalSpecColor = float4(0, 0, 0, 0);
	float4 pointColor = float4(0, 0, 0, 0);
	float4 pointSpecColor = float4(0, 0, 0, 0);
	//float3 spotColor = float3(0, 0, 0);

	//calculate dircolor
	float4 ambientColor = ambientLight;
	float3 dirDirection = dirLightNorm.xyz;

	float dirDiffuseFactor = dot(Normal, dirDirection);

	if (dirDiffuseFactor > 0.0f || diffuseColor.a < 1.0f)
	{
		directionalColor = float4(dirLightColor.xyz, 1.0f) * dirDiffuseFactor;

		float3 lightRef = normalize(reflect(-dirLightNorm.xyz, Normal));
		float specFactor = dot(ViewVector, lightRef);
		if (specFactor > 0)
		{
			specFactor = pow(specFactor, 128);
			directionalSpecColor = float4(dirLightColor.xyz * specMap.xyz * specFactor, 1.0f);
		}
	}

	for (int i = 0; i < NUMOFPOINTLIGHTS; ++i) 
	{
		float3 lightDir = normalize(pLights[i].pointLightPosition.xyz - input.worldPosition.xyz);
		float lightFactor = saturate(dot(lightDir, Normal));
		if (lightFactor > 0)
		{
			pointColor += lightFactor * pLights[i].pointLightColor;
		}

		float3 lightRef = normalize(reflect(lightDir, Normal));
		float specFactor = dot(ViewVector, lightRef);
		if (specFactor > 0)
		{
			specFactor = pow(specFactor, 24);
			pointSpecColor += pLights[i].pointLightColor * specMap * specFactor * lightFactor;
		}
	}
	
	float4 emissiveColor = float4(0, 0, 0, 0);
	if (emisMap.r > 0)
	{
		if (emisMap.r > 0.26f)
		{
			emisMap.r += 0.185f;
		}
		emissiveColor.rgba += TeamColorB.rgba * emisMap.r;
	}

	float4 finalColor = diffuseColor * ((ambientColor + directionalColor + directionalSpecColor + pointColor + pointSpecColor) * black);
	finalColor = emissiveColor + finalColor * (1 - emissiveColor.a);
	finalColor.a = diffuseColor.a + emissiveColor.a;
	return saturate(finalColor);
}