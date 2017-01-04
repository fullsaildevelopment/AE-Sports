#include "AnimatedRenderNode.h"

//constructor
AnimatedRenderNode::AnimatedRenderNode()
{
	//initialize everything to -1, which means it wasn't set
	indexBuffer = -1;
	vertexBuffer = -1;
	vertexSlice = -1; 
	numOfVertices = -1;
	inputLayout = -1;
	pixelShader = -1;
	vertexShader = -1;
	computeShader = -1;
}

//basic
void AnimatedRenderNode::Init(std::string mesh, std::string psName, std::string vsName, std::string csName)
{

}

void AnimatedRenderNode::Update()
{

}

//getters//

std::vector<DirectX::XMFLOAT4X4>& AnimatedRenderNode::GetBoneOffsets()
{ 
	return blender.GetBoneOffsets();
}

std::vector<DirectX::XMFLOAT4X4>& AnimatedRenderNode::GetBonesWorlds()
{ 
	return blender.GetBonesWorlds();
}