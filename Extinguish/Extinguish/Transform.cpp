#include <iostream>
#include "Transform.h"
#include "GameObject.h"

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

	defaultP.push_back(pos);
	defaultP.push_back(rot);
	defaultP.push_back(tempScale);

	moveTotalTime = -1;
	lookTotalTime = -1;
	rotateTotalTime = -1;

	velocity = { 0, 0, 0 };
}

void Transform::Reset()
{
	SetPosition(defaultP[0]);
	SetRotation(defaultP[1]);
	SetScale(defaultP[2]);
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

void Transform::RemoveChildren()
{
	for (int i = 0; i < children.size(); ++i)
	{
		children[i]->SetParent(nullptr);
	}

	children.clear();
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

float3 Transform::GetWorldPosition()
{
	DirectX::XMFLOAT4X4 world = GetWorld();
	return float3(world._41, world._42, world._43);
}

void Transform::FixedUpdate(float _dt)
{
	//Translate({ velocity.x * dt, velocity.y * dt, velocity.z * dt });

	lookCurTime += _dt;
	moveCurTime += _dt;
	rotateCurTime += _dt;

	if (moveTotalTime != -1)
	{
		Move();
	}

	if (lookTotalTime != -1)
	{
		Look();
	}

	if (rotateTotalTime != -1)
	{
		Rotate();
	}
}

void Transform::LookAt(float3 pos, float totalTime)
{
	lookPosition = pos;
	lookTotalTime = totalTime;
	lookCurTime = 0.0f;

	//store position for lerp
	originalLookPosition = GetPosition();

	//store forward
	originalForward = GetForwardf3();

	//rotation needed
	originalLookRotation = GetRotation();
}

void Transform::RotateTo(float3 rotation, float totalTime)
{
	rotateRotation = rotation;
	rotateTotalTime = totalTime;
	rotateCurTime = 0.0f;

	//store rotation for lerp
	originalRotation = GetRotation();
}


void Transform::MoveTo(float3 pos, float totalTime)
{
	moveDestination = pos;
	moveTotalTime = totalTime;
	moveCurTime = 0.0f;

	//store position for lerp
	originalPosition = GetPosition();
}

//setters
void Transform::BDirty()
{
	//only mark as dirty if it's not already dirty. This makes sure you don't mark children as dirty unnecessarily
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
	SetPosition({ matrix._41, matrix._42, matrix._43 });
	XMVECTOR qua;
	XMVECTOR s;
	XMVECTOR t;
	XMMATRIX m = XMLoadFloat4x4(&matrix);
	XMMatrixDecompose(&s, &qua, &t, m);
	XMFLOAT4 scale;
	XMStoreFloat4(&scale, s);
	SetScale({ scale.x,scale.y,scale.z });
	XMFLOAT4 qu;
	XMStoreFloat4(&qu, qua);
	float4 q;
	q = float4(qu.w, qu.x, qu.y, qu.z);

	double ysqr = q.y * q.y;

	double t0 = 2.0 * (q.w * q.x + q.y * q.z);
	double t1 = 1.0 - 2.0 * (q.x *q.x + ysqr);
	rotation.x = std::atan2(t0, t1);

	double t2 = 2.0 * (q.w * q.y - q.z * q.x);
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	rotation.y = std::asin(t2);

	double t3 = 2.0 * (q.w * q.z + q.x * q.y);
	double t4 = 1.0 - 2.0 * (ysqr + q.z * q.z);
	rotation.z = atan2(t3, t4);

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

		XMMATRIX tempLocal = XMMatrixMultiply(tempScale, tempRotation);
		tempLocal = XMMatrixMultiply(tempLocal, tempTranslation);

		XMStoreFloat4x4(&local, tempLocal);

		XMMATRIX tempWorld;

		if (parent)
		{
			tempWorld = XMMatrixMultiply(tempLocal, XMLoadFloat4x4(&parent->GetWorld()));
			//tempWorld = XMMatrixMultiply(XMLoadFloat4x4(&parent->GetWorld()), tempWorld);
		}
		else
		{
			tempWorld = tempLocal;
		}
		
		//world = local;

		XMStoreFloat4x4(&world, tempWorld);

		bDirty = false;
	}
	//else
	//{
	//	world = local;
	//}

	return world;
}

DirectX::XMFLOAT4X4* Transform::GetWorldP()
{
	GetWorld();

	return &world;
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

float3 Transform::GetForwardf3()
{
	return float3(local._31, local._32, local._33).normalize();
}

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

float3 Transform::GetRightf3()
{
	return float3(local._11, local._12, local._13).normalize();
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
	return { XMConvertToDegrees(rotation.x), XMConvertToDegrees(rotation.y), XMConvertToDegrees(rotation.z) };
}

//private helper functions//
void Transform::Move()
{
	float ratio = moveCurTime / moveTotalTime;

	if (ratio <= 1.0f)
	{
		float3 newPosition;

		newPosition.x = (moveDestination.x - originalPosition.x) * ratio + originalPosition.x;
		newPosition.y = (moveDestination.y - originalPosition.y) * ratio + originalPosition.y;
		newPosition.z = (moveDestination.z - originalPosition.z) * ratio + originalPosition.z;

		SetPosition(newPosition);

		if (GetGameObject()->GetName() == "Mage1")
		{
			int breakPoint = 69;
			breakPoint = breakPoint;
		}
	}
	else
	{
		moveTotalTime = -1; //no more moving
	}
}

void Transform::Look()
{
	float ratio = lookCurTime / lookTotalTime;

	if (ratio <= 1.0f)
	{
		float3 newRotation;

		float3 newDirection = lookPosition - originalLookPosition;
		newDirection.normalize();

		float radian = acos(dot_product(newDirection, originalForward));
		float3 axis;
		cross_product(axis, newDirection, originalForward);
		axis.normalize();

		SetRotation({ axis.x * radian * ratio + originalLookRotation.x, -axis.y * radian * ratio + originalLookRotation.y, axis.z * radian * ratio + originalLookRotation.z });
	}
	else
	{
		lookTotalTime = -1; //no more looking
	}
}

void Transform::Rotate()
{
	float ratio = rotateCurTime / rotateTotalTime;

	if (ratio <= 1.0f)
	{
		float3 newRotation;

		newRotation.x = (rotateRotation.x - originalRotation.x) * ratio + originalRotation.x;
		newRotation.y = (rotateRotation.y - originalRotation.y) * ratio + originalRotation.y;
		newRotation.z = (rotateRotation.z - originalRotation.z) * ratio + originalRotation.z;

		SetRotation(newRotation);
	}
	else
	{
		rotateTotalTime = -1; //no more looking
	}
}