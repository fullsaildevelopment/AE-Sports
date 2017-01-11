#include "Transform.h"
#include <iostream>
using namespace DirectX;
using namespace std;
Transform::Transform() : Component(nullptr)
{
	bDirty = true;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 1, 1, 1 };

	parent = nullptr;
	child = nullptr;
	sibling = nullptr;
}

Transform::Transform(GameObject* o) : Component(o)
{
	bDirty = false;
	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 0, 0, 0 };
}

Transform::~Transform()
{

}

//basic
void Transform::Init(DirectX::XMFLOAT4X4 localMatrix, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 tempScale, Transform* parent, Transform* child, Transform* sibling)
{
	local = localMatrix;
	SetPosition(pos);
	SetRotation(rot);
	SetScale(tempScale);
	SetParent(parent);
	AddChild(child);
	AddSibling(sibling);
}

//misc
void Transform::Translate(DirectX::XMFLOAT3 vector)
{
	position.x += vector.x;
	position.y += vector.y;
	position.z += vector.z;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	XMMATRIX translation = XMMatrixTranslation(vector.x, vector.y, vector.z);
	tempLocal = XMMatrixMultiply(XMMatrixTranslation(vector.x, vector.y, vector.z), tempLocal);
	XMStoreFloat4x4(&local, tempLocal);

	//local._41 = position.x;
	//local._42 = position.y;
	//local._43 = position.z;

	cout << vector.x << " " << vector.y << " " << vector.z << endl;

	if (child)
	{
		child->BDirty();
	}
}

void Transform::RotateX(float degrees)
{
	rotation.x += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(XMMatrixRotationX(degrees), tempLocal);
	XMStoreFloat4x4(&local, tempLocal);

	if (child)
	{
		child->BDirty();
	}
}

void Transform::RotateY(float degrees)
{
	rotation.y += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(XMMatrixRotationY(degrees), tempLocal);
	XMStoreFloat4x4(&local, tempLocal);

	if (child)
	{
		child->BDirty();
	}
}

void Transform::RotateZ(float degrees)
{
	rotation.z += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(XMMatrixRotationZ(degrees), tempLocal);
	XMStoreFloat4x4(&local, tempLocal);

	if (child)
	{
		child->BDirty();
	}
}

void Transform::AddChild(Transform* tempChild)
{
	if (!child)
	{
		child = tempChild;
	}
	else
	{
		//if there's already a child, give that child a sibling
		child->AddSibling(tempChild);
	}
}

void Transform::AddSibling(Transform* tempSibling)
{
	if (!sibling)
	{
		sibling = tempSibling;
	}
	else
	{
		sibling->AddSibling(tempSibling);
	}
}

//setters
void Transform::BDirty()
{
	bDirty = true;

	if (child)
	{
		child->BDirty();
	}
}

void Transform::SetScale(DirectX::XMFLOAT3 vector)
{
	scale = vector;

	XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX tempLocal = XMMatrixMultiply(tempScale, XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y)), XMMatrixRotationZ(rotation.z)));
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixTranslation(position.x, position.y, position.z));
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::SetPosition(DirectX::XMFLOAT3 vector) 
{
	position = vector;

	//XMMATRIX tempPosition = XMMatrixTranslation(position.x, position.y, position.z);
	//XMMATRIX tempLocal;
	//tempLocal = XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y));
	//tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationZ(rotation.z));
	//tempLocal = XMMatrixMultiply(tempLocal, XMMatrixScaling(scale.x, scale.y, scale.z));
	//tempLocal = XMMatrixMultiply(tempLocal, tempPosition);
	//XMStoreFloat4x4(&local, tempLocal);

	local._41 = position.x;
	local._42 = position.y;
	local._43 = position.z;

	if (child)
	{
		child->BDirty();
	}
}

void Transform::SetRotation(DirectX::XMFLOAT3 vector)
{
	rotation = vector;

	XMMATRIX tempLocal;
	tempLocal = XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y));
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationZ(rotation.z));
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixScaling(scale.x, scale.y, scale.z));
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixTranslation(position.x, position.y, position.z));
	XMStoreFloat4x4(&local, tempLocal);

	if (child)
	{
		child->BDirty();
	}
}

void Transform::SetWorld(DirectX::XMFLOAT4X4 matrix)
{
	world = matrix;
}

void Transform::SetLocal(DirectX::XMFLOAT4X4 matrix)
{
	local = matrix;

	BDirty();
}

void Transform::SetParent(Transform* pParent)
{
	//set parent
	parent = pParent;

	//give the parent a child
	if (pParent)
	{
		parent->AddChild(this);
	}
}

//getters
DirectX::XMFLOAT4X4 Transform::GetWorld()
{
	if (parent && bDirty)
	{
		XMMATRIX tempLocal = XMLoadFloat4x4(&local);
		XMMATRIX tempParentWorld = XMLoadFloat4x4(&parent->GetWorld());
		XMMATRIX tempWorld = (XMMatrixMultiply(tempLocal, tempParentWorld));

		XMStoreFloat4x4(&world, tempWorld);

		bDirty = false;
	}
	else if (!parent)
	{
		XMMATRIX tempLocal = XMLoadFloat4x4(&local);
		XMStoreFloat4x4(&world, (tempLocal));
	}

	return world;
}

DirectX::XMFLOAT4X4 Transform::GetLocal()
{
	return local;
}

bool Transform::GetBDirty()
{
	return bDirty;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

Transform* Transform::GetParent()
{
	return parent;
}

Transform* Transform::GetChild()
{
	return child;
}

Transform* Transform::GetSibling()
{
	return sibling;
}