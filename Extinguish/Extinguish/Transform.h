#pragma once
#include <DirectXMath.h>
#include <string>

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

	FriendlyIOTransformNode();
	//void AddChild(TransformNode2* tempChild);
};

class Transform
{
private:
	std::string name;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 local;
	Transform* parent;
	Transform* child;
	Transform* sibling;
	bool bDirty; //if you	 change the world of the parent, make all of its children dirty. This means their world needs to be updated. This is beneficial because you could make your children dirty multiple times before you actually update world, thus, saving processing

	//private helper functions
	//void AddSibling(TransformNode2* tempSibling);

	//void AddChild(TransformNode2* tempChild);
public:
	Transform();

	//misc
	void Translate(DirectX::XMFLOAT3 vector);
	void Rotate(DirectX::XMFLOAT3 vector);
	void AddChild(Transform* tempChild);
	void AddSibling(Transform* tempSibling);

	//setters
	void SetBDirty();
	void SetScale(DirectX::XMFLOAT3 vector);
	void SetPosition(DirectX::XMFLOAT3 vector);
	void SetRotation(DirectX::XMFLOAT3 vector);
	void SetWorld(DirectX::XMFLOAT4X4 matrix);
	void SetLocal(DirectX::XMFLOAT4X4 matrix);

	//getters
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT4X4 GetLocal();
	bool GetBDirty();
};