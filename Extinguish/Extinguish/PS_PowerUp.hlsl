struct PS_BasicInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : TEXCOORD;
	float4 worldPosition : POSITION0;
};

cbuffer EmissiveColorCB : register(b3)
{
	float4 EmissiveColorB;
}

texture2D baseTexture : register(t0);
texture2D emissiveMap : register(t3);

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{

	float4 diffuseColor = baseTexture.Sample(filter, input.uv.xy);
	
	float4 emisMap = emissiveMap.Sample(filter, input.uv.xy);
	float4 emissiveColor = float4(0, 0, 0, 0);
	if (emisMap.r > 0)
	{
		if (emisMap.r > 0.26f)
		{
			emisMap.r += 0.185f;
		}
		emissiveColor.rgba += EmissiveColorB.rgba * emisMap.r;
	}

	float4 finalColor = diffuseColor;
	finalColor += emissiveColor;
	finalColor.a = diffuseColor.a + emissiveColor.a;
	return saturate(finalColor);
}