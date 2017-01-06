#include "Transform.h"

using namespace DirectX;

FriendlyIOTransformNode::FriendlyIOTransformNode()
{
	//nameOffset = 0;
	world = DirectX::XMFLOAT4X4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	local = DirectX::XMFLOAT4X4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	parentIndex = -1;
	childIndex = -1;
	siblingIndex = -1;
	bDirty = false;
}

void Transform::Translate(DirectX::XMFLOAT3 vector)
{
	position.x += vector.x;
	position.y += vector.y;
	position.z += vector.z;
}

void Transform::Rotate(DirectX::XMFLOAT3 vector)
{
	rotation.x += vector.x;
	rotation.y += vector.y;
	rotation.z += vector.z;
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
}

void Transform::SetPosition(DirectX::XMFLOAT3 vector)
{
	position = vector;
}

void Transform::SetRotation(DirectX::XMFLOAT3 vector)
{
	rotation = vector;
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
DirectX::XMFLOAT4X4 Transform::GetWorld()
{
	if (parent && bDirty)
	{
		XMMATRIX tempLocal = XMLoadFloat4x4(&local);
		XMMATRIX tempParentWorld = XMLoadFloat4x4(&parent->world);
		XMMATRIX tempWorld = XMMatrixMultiply(tempLocal, tempParentWorld);

		XMStoreFloat4x4(&world, tempWorld);
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