#include <iostream>
#include "Transform.h"

using namespace DirectX;
using namespace std;

Transform::Transform() : Component(nullptr)
{
	bDirty = true;

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 1, 1, 1 };

	parent = nullptr;
	//child = nullptr;
	//sibling = nullptr;
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
void Transform::Init(DirectX::XMFLOAT4X4 localMatrix, float3 pos, float3 rot, float3 tempScale, Transform* parent, Transform* child, Transform* sibling)
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

	//XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	//XMMATRIX translation = XMMatrixTranslation(vector.x, vector.y, vector.z);
	//tempLocal = XMMatrixMultiply(XMMatrixTranslation(vector.x, vector.y, vector.z), tempLocal);
	//XMStoreFloat4x4(&local, tempLocal);

	////local._41 = position.x;
	////local._42 = position.y;
	////local._43 = position.z;

	////cout << vector.x << " " << vector.y << " " << vector.z << endl;

	BDirty();
}

void Transform::RotateX(float degrees)
{
	rotation.x += degrees;

	//XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	//tempLocal = XMMatrixMultiply(XMMatrixRotationX(degrees), tempLocal);
	//XMStoreFloat4x4(&local, tempLocal);

	BDirty();
}

void Transform::RotateY(float degrees)
{
	rotation.y += degrees;

	//XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	//tempLocal = XMMatrixMultiply(XMMatrixRotationY(degrees), tempLocal);
	//XMStoreFloat4x4(&local, tempLocal);

	BDirty();
}

void Transform::RotateZ(float degrees)
{
	rotation.z += degrees;

	//XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	//tempLocal = XMMatrixMultiply(XMMatrixRotationZ(degrees), tempLocal);
	//XMStoreFloat4x4(&local, tempLocal);

	BDirty();
}

void Transform::AddChild(Transform* tempChild)
{
	//if (!child)
	//{
	//	child = tempChild;
	//}
	//else
	//{
	//	//if there's already a child, give that child a sibling
	//	child->AddSibling(tempChild);
	//}

	if (tempChild)
	{
		children.push_back(tempChild);
	}
}

void Transform::RemoveChild(Transform* abortee)
{
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i] == abortee)
		{
			children.erase(children.begin() + i);
			break;
		}
	}
}

void Transform::AddSibling(Transform* tempSibling)
{
	//if (!sibling)
	//{
	//	sibling = tempSibling;
	//}
	//else
	//{
	//	sibling->AddSibling(tempSibling);
	//}
}

void Transform::Update(float dt)
{
	Translate({ velocity.x * dt, velocity.y * dt, velocity.z * dt });
	//velocity.x *= 0.8;
	//velocity.y *= 0.8;
	//velocity.z *= 0.8;
}

//setters
void Transform::BDirty()
{
	//only mark as dirty if it's not already dirty. This makes sure you don't mark children as dirty unecessarily
	if (!bDirty)
	{
		bDirty = true;

		for (int i = 0; i < children.size(); ++i)
		{
			if (children[i])
			{
				children[i]->BDirty();
			}
		}
	}
}

void Transform::SetScale(float3 vector)
{
	scale = vector;

	BDirty();

	//XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//XMMATRIX tempLocal = XMMatrixMultiply(tempScale, XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y)), XMMatrixRotationZ(rotation.z)));
	//tempLocal = XMMatrixMultiply(tempLocal, XMMatrixTranslation(position.x, position.y, position.z));
	//XMStoreFloat4x4(&local, tempLocal);
}

void Transform::SetPosition(float3 vector)
{
	position = vector;

	//XMMATRIX tempPosition = XMMatrixTranslation(position.x, position.y, position.z);
	//XMMATRIX tempLocal;
	//tempLocal = XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y));
	//tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationZ(rotation.z));
	//tempLocal = XMMatrixMultiply(tempLocal, XMMatrixScaling(scale.x, scale.y, scale.z));
	//tempLocal = XMMatrixMultiply(tempLocal, tempPosition);
	//XMStoreFloat4x4(&local, tempLocal);

	////local._41 = position.x;
	////local._42 = position.y;
	////local._43 = position.z;

	BDirty();
}

void Transform::SetRotation(float3 vector)
{
	rotation = vector;

	//XMMATRIX tempLocal;
	//tempLocal = XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y));
	//tempLocal = XMMatrixMultiply(XMMatrixRotationZ(rotation.z), tempLocal);
	//tempLocal = XMMatrixMultiply(XMMatrixScaling(scale.x, scale.y, scale.z), tempLocal);
	//tempLocal = XMMatrixMultiply(XMMatrixTranslation(position.x, position.y, position.z), tempLocal);
	//XMStoreFloat4x4(&local, tempLocal);

	BDirty();
}

void Transform::SetWorld(DirectX::XMFLOAT4X4 matrix)
{
	world = matrix;
}

void Transform::SetLocal(DirectX::XMFLOAT4X4 matrix)
{
	local = matrix;

	//BDirty();
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

void Transform::SetVelocity(float3 v)
{
	velocity = v;
}

void Transform::AddVelocity(float3 a)
{
	velocity += a;
}

//getters
DirectX::XMFLOAT4X4 Transform::GetWorld()
{
	if (bDirty)
	{
		XMMATRIX identity = XMMatrixIdentity();
		XMMATRIX tempRotation = XMMatrixMultiply(identity, XMMatrixRotationX(rotation.x));
		tempRotation = XMMatrixMultiply(tempRotation, XMMatrixRotationY(rotation.y));
		tempRotation = XMMatrixMultiply(tempRotation, XMMatrixRotationZ(rotation.z));
		XMMATRIX tempTranslation = XMMatrixMultiply(identity, XMMatrixTranslation(position.x, position.y, position.z));
		XMMATRIX tempScale = XMMatrixMultiply(identity, XMMatrixScaling(scale.x, scale.y, scale.z));

		XMMATRIX tempWorld = XMMatrixMultiply(tempScale, tempRotation);
		tempWorld = XMMatrixMultiply(tempWorld, tempTranslation);

		if (parent)
		{
			tempWorld = XMMatrixMultiply(tempWorld, XMLoadFloat4x4(&parent->GetWorld()));
			//tempWorld = XMMatrixMultiply(XMLoadFloat4x4(&parent->GetWorld()), tempWorld);
		}
		
		XMStoreFloat4x4(&local, tempWorld);

		world = local;

		bDirty = false;
	}
	else
	{
		world = local;
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

float3 Transform::GetPosition()
{
	return position;
}

float3 Transform::GetRotation()
{
	return rotation;
}

float3 Transform::GetScale()
{
	return scale;
}

Transform* Transform::GetParent()
{
	return parent;
}

Transform* Transform::GetChild(int index)
{
	return children[index];
}

//Transform* Transform::GetSibling()
//{
//	return sibling;
//}

DirectX::XMFLOAT3 Transform::GetForward()
{
	XMFLOAT3 result;
	XMVECTOR vector = { local._31, local._32, local._33 };

	vector = XMVector3Normalize(vector);
	XMStoreFloat3(&result, vector);

	return result;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	XMFLOAT3 result;
	XMVECTOR vector = { local._21, local._22, local._23 };

	vector = XMVector3Normalize(vector);
	XMStoreFloat3(&result, vector);

	return result;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	XMFLOAT3 result;
	XMVECTOR vector = { local._11, local._12, local._13 };

	vector = XMVector3Normalize(vector);
	XMStoreFloat3(&result, vector);

	return result;
}

float3 Transform::GetVelocity()
{
	return velocity;
}

DirectX::XMFLOAT3 Transform::GetRotationDeg()
{
	return{ XMConvertToDegrees(rotation.x), XMConvertToDegrees(rotation.y), XMConvertToDegrees(rotation.z) };
}
