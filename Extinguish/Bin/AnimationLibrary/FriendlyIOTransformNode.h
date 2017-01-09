#pragma once
#include <DirectXMath.h>

//this struct is used for exporting to binary in a friendly manner
struct FriendlyIOTransformNode
{
	//std::string name;
	unsigned int nameOffset;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 local;
	int parentIndex;
	int childIndex;
	int siblingIndex;
	bool bDirty; //if you	 change the world of the parent, make all of its children dirty. This means their world needs to be updated. This is beneficial because you could make your children dirty multiple times before you actually update world, thus, saving processing

				 //private helper functions
				 //void AddSibling(TransformNode2* tempSibling);

	FriendlyIOTransformNode()
	{
		//nameOffset = 0;
		world = DirectX::XMFLOAT4X4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		local = DirectX::XMFLOAT4X4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		parentIndex = -1;
		childIndex = -1;
		siblingIndex = -1;
		bDirty = false;
	}
	//void AddChild(TransformNode2* tempChild);
};