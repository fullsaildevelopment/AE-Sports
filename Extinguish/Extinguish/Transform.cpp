#include "Transform.h"
#include <iostream>
using namespace DirectX;
using namespace std;
Transform::Transform()
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
void Transform::Init(XMFLOAT4X4 matrix, string txt)
{
	local = matrix;
	name = txt;
}

//misc
void Transform::Translate(DirectX::XMFLOAT3 vector)
{
	position.x += vector.x;
	position.y += vector.y;
	position.z += vector.z;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	XMMATRIX translation = XMMatrixTranslation(vector.x, vector.y, vector.z);
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixTranslation(vector.x, vector.y, vector.z));
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::RotateX(float degrees)
{
	rotation.x += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationX(degrees));
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::RotateY(float degrees)
{
	rotation.y += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationY(degrees));
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::RotateZ(float degrees)
{
	rotation.z += degrees;

	XMMATRIX tempLocal = XMLoadFloat4x4(&local);
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationZ(degrees));
	XMStoreFloat4x4(&local, tempLocal);
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
void Transform::SetBDirty()
{
	bDirty = true;

	if (child)
	{
		child->SetBDirty();
	}
}


void Transform::SetScale(DirectX::XMFLOAT3 vector)
{
	scale = vector;

	XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX tempLocal;
	tempLocal = XMMatrixMultiply(XMMatrixIdentity(), tempScale);
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::SetPosition(DirectX::XMFLOAT3 vector)
{
	position = vector;

	XMMATRIX tempPosition = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX tempLocal;
	tempLocal = XMMatrixMultiply(XMMatrixIdentity(), tempPosition);
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::SetRotation(DirectX::XMFLOAT3 vector)
{
	rotation = vector;

	XMMATRIX tempLocal;
	tempLocal = XMMatrixMultiply(XMMatrixRotationX(rotation.x), XMMatrixRotationY(rotation.y));
	tempLocal = XMMatrixMultiply(tempLocal, XMMatrixRotationZ(rotation.z));
	XMStoreFloat4x4(&local, tempLocal);
}

void Transform::SetWorld(DirectX::XMFLOAT4X4 matrix)
{
	world = matrix;
}

void Transform::SetLocal(DirectX::XMFLOAT4X4 matrix)
{
	local = matrix;
}

//getters

//I transpose the world only
DirectX::XMFLOAT4X4 Transform::GetWorld()
{
	if (parent && bDirty)
	{
		XMMATRIX tempLocal = XMLoadFloat4x4(&local);
		XMMATRIX tempParentWorld = XMLoadFloat4x4(&parent->world);
		XMMATRIX tempWorld = XMMatrixTranspose(XMMatrixMultiply(tempLocal, tempParentWorld));

		XMStoreFloat4x4(&world, tempWorld);

		bDirty = false;
	}
	else
	{
		XMMATRIX tempLocal = XMLoadFloat4x4(&local);
		XMStoreFloat4x4(&world, XMMatrixTranspose(tempLocal));
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