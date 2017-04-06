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
void main(lineadj GSInput input[4], inout TriangleStream< GSOutput > output)
{
	float3 p12p2 = input[1].pos.xyz - input[0].pos.xyz;
	float3 p22p3 = input[2].pos.xyz - input[1].pos.xyz;
	float3 p32p4 = input[3].pos.xyz - input[2].pos.xyz;
	float3 normDirLeft = normalize(p12p2);
	float3 normDir = normalize(p22p3);
	float3 normDirRight = normalize(p32p4);
	float3 up = float3(0, 1, 0);
	float dotDirUpLeft = dot(normDirLeft, up);
	float dotDirUp = dot(normDir, up);
	float dotDirUpRight = dot(normDirRight, up);
	if (dotDirUpLeft == 1 || dotDirUpLeft == -1)
	{
		normDirLeft += 0.00001f;
	}
	if (dotDirUp == 1 || dotDirUp == -1)
	{
		normDir += 0.00001f;
	}
	if (dotDirUpRight == 1 || dotDirUpRight == -1)
	{
		normDirRight += 0.00001f;
	}
	float3 rightAxisLeft = cross(normDirLeft, up);
	float3 rightAxis = cross(normDir, up);
	float3 rightAxisRight = cross(normDirRight, up);
	float3 upAxis = normalize(cross(rightAxis, normDir));
	float3 upAxisLeft = normalize(cross(rightAxisLeft, normDirLeft));
	float3 upAxisRight = normalize(cross(rightAxisRight, normDirRight));
	upAxisLeft = normalize(upAxisLeft + upAxis);
	upAxisRight = normalize(upAxisRight + upAxis);
	GSOutput element[4] = { { 0,0,0,1 }, { 0,0,0,1 }, { 0,0,0,1 }, { 0,0,0,1 } };
	element[0].pos.xyz = input[1].pos.xyz + upAxisLeft * (-input[1].size);
	element[0].pos = mul(element[0].pos, view);
	element[0].pos = mul(element[0].pos, projection);
	output.Append(element[0]);
	element[1].pos.xyz = input[1].pos.xyz + upAxisLeft * (input[1].size);
	element[1].pos = mul(element[1].pos, view);
	element[1].pos = mul(element[1].pos, projection);
	output.Append(element[1]);
	element[2].pos.xyz = input[2].pos.xyz + upAxisRight * (-input[2].size);
	element[2].pos = mul(element[2].pos, view);
	element[2].pos = mul(element[2].pos, projection);
	output.Append(element[2]);
	element[3].pos.xyz = input[2].pos.xyz + upAxisRight * (input[2].size);
	element[3].pos = mul(element[3].pos, view);
	element[3].pos = mul(element[3].pos, projection);
	output.Append(element[3]);
}