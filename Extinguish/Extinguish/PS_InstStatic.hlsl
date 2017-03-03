
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
	
	if (diffuseColor.r > 0.25f)
	{
		finalColor.r = ((0xFF000000 & input.color) >> 24) * 0.003906f;
		finalColor.g = ((0x00FF0000 & input.color) >> 16) * 0.003906f;
		finalColor.b = ((0x0000FF00 & input.color) >> 8) * 0.003906f;
	}
	finalColor.a = 1;
	return finalColor;
}