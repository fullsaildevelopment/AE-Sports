#include "Renderer.h"

//constructor
Renderer::Renderer()
{
	//initialize everything to -1, which means it wasn't set
	//indexBuffer = -1;
	//vertexBuffer = -1;
	////vertexSlice = -1; 
	//numOfVertices = -1;
	//inputLayout = -1;
	//pixelShader = -1;
	//vertexShader = -1;
	//computeShader = -1;
}

//basic
void Renderer::Init(std::string mesh, std::string psName, std::string vsName, std::string csName, ResourceManager* resources)
{
	indexBuffer = resources->GetIndexBuffer(mesh);
	vertexBuffer = resources->GetVertexBuffer(mesh);
	inputLayout = resources->GetInputLayout(vsName);
	pixelShader = resources->GetPixelShader(psName);
	vertexShader = resources->GetVertexShader(vsName);
	computeShader = resources->GetComputeShader(csName);
}

void Renderer::Update()
{

}

//getters//

std::vector<DirectX::XMFLOAT4X4>& Renderer::GetBoneOffsets()
{ 
	return blender.GetBoneOffsets();
}

std::vector<DirectX::XMFLOAT4X4>& Renderer::GetBonesWorlds()
{ 
	return blender.GetBonesWorlds();
}