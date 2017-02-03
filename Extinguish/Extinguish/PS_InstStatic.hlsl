
struct PS_BasicInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	uint color : COLOR;
};

texture2D baseTexture : register(t0);

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{
	float4 finalColor = float4(0,0,0,0);

	//get texture color with uvs help
	float3 diffuseColor = (float3)baseTexture.Sample(filter, input.uv);
	
	if (diffuseColor.r > 0)
	{
		finalColor.r = (input.color & 0xFF000000) * 0.003921;
		finalColor.g = (input.color & 0x00FF0000) * 0.003921;
		finalColor.b = (input.color & 0x0000FF00) * 0.003921;
		finalColor.a = 1.0;
	}

	return finalColor;
}