struct PS_BasicInput
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

cbuffer ScrollOffsetCB : register(b0)
{
	float4 ScrollOffsets;
}

cbuffer ShieldColorCB : register(b1)
{
	float4 ShieldColorB;
}


texture2D baseTexture : register(t0); //ShieldDiffuse - not needed just used for testing
texture2D NoiseMap1 : register(t1); //NoiseMap 1
texture2D NoiseMap2 : register(t2); //NoiseMap 2

SamplerState filter : register(s0);

float4 main(PS_BasicInput input) : SV_TARGET
{
	//Offsetting uv's for noise map 1
	float2 cord1 = input.uv * 0.75f;
	cord1.x += ScrollOffsets.x;
	//Offsetting uv's for noise map 2
	float2 cord2 = input.uv * 9.0f;
	cord2.y += ScrollOffsets.y;
	//Get where we are in the first noise map + Offset
	float noiseSample1 = NoiseMap1.Sample(filter, cord1).x;
	//Get where we are in the second noise map + Offset
	float noiseSample2 = NoiseMap2.Sample(filter, cord2).x;

	float2 cord3 = (input.uv * 2.75f) - (ScrollOffsets.x + ScrollOffsets.y);
	float noiseSample = baseTexture.Sample(filter, cord3).x;

	float totalNoise = (noiseSample1 * 0.5f + noiseSample2 * 0.356f + noiseSample * 0.5f);

	float masking = saturate(((totalNoise - 0.267f) / (1 - 0.267f)) * 1.85f);

	float4 finalColor = ShieldColorB * masking;

	finalColor += ShieldColorB * 0.25f;

	return saturate(finalColor);
}