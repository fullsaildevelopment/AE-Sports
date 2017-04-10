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
	float2 cord1 = input.uv;
	cord1 += ScrollOffsets.x;
	//Offsetting uv's for noise map 2
	float2 cord2 = input.uv;
	cord2 += ScrollOffsets.y;
	//Get where we are in the first noise map + Offset
	float noiseSample1 = NoiseMap1.Sample(filter, cord1).x;
	//Get where we are in the second noise map + Offset
	float noiseSample2 = NoiseMap2.Sample(filter, cord2).x;

	float totalNoise = noiseSample1 + noiseSample2;

	float4 baseColor = baseTexture.Sample(filter, input.uv);

	return baseColor + ShieldColorB * totalNoise;
}