cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
};

struct GSInput
{
	float4 pos : SV_POSITION;
	float size : TRAILSIZE;
};

[maxvertexcount(4)]
void main(line GSInput input[2], inout TriangleStream< GSOutput > output)
{
	float3 p12p2 = input[1].pos.xyz - input[0].pos.xyz;
	float3 normDir = normalize(p12p2);
	float3 up = float3(0, 1, 0);
	float dotDirUp = dot(normDir, up);
	if (dotDirUp == 1 || dotDirUp == -1)
	{
		normDir += 0.00001f;
	}
	float3 rightAxis = cross(normDir, up);
	float3 upAxis = normalize(cross(rightAxis, normDir));
	GSOutput element[4] = { { 0,0,0,1 }, { 0,0,0,1 }, { 0,0,0,1 }, { 0,0,0,1 } };
	element[0].pos.xyz = input[0].pos.xyz + upAxis * (-input[0].size);
	element[0].pos = mul(element[0].pos, view);
	element[0].pos = mul(element[0].pos, projection);
	output.Append(element[0]);
	element[1].pos.xyz = input[0].pos.xyz + upAxis * (input[0].size);
	element[1].pos = mul(element[1].pos, view);
	element[1].pos = mul(element[1].pos, projection);
	output.Append(element[1]);
	element[2].pos.xyz = input[1].pos.xyz + upAxis * (-input[0].size);
	element[2].pos = mul(element[2].pos, view);
	element[2].pos = mul(element[2].pos, projection);
	output.Append(element[2]);
	element[3].pos.xyz = input[1].pos.xyz + upAxis * (input[0].size);
	element[3].pos = mul(element[3].pos, view);
	element[3].pos = mul(element[3].pos, projection);
	output.Append(element[3]);
}