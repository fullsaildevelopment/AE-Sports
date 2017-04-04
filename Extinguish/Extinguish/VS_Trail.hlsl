// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 pos : POSITION;
	float size : SIZE;
};

// Per-pixel color data passed through the pixel shader.
struct GSInput
{
	float4 pos : SV_POSITION;
	float size : TRAILSIZE;
};

// Simple shader to do vertex processing on the GPU.
GSInput main(VertexShaderInput input)
{
	GSInput output;

	output.pos = input.pos;
	output.size = input.size;

	return output;
}