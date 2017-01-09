#include "CapsuleCollider.h"

using namespace DirectX;

Capsule CapsuleCollider::GetCapsule()
{
	Capsule cap;
	cap.m_Radius = radius;
	cap.m_Segment.m_Start.x = Start.x;
	cap.m_Segment.m_Start.y = Start.y;
	cap.m_Segment.m_Start.z = Start.z;
	cap.m_Segment.m_End.x = End.x;
	cap.m_Segment.m_End.y = End.y;
	cap.m_Segment.m_End.z = End.z;
	return cap;
}

Capsule CapsuleCollider::GetWorldCapsule()
{
	Capsule cap = GetCapsule();
	//cap.m_Segment.m_Start.x += GetTransform().GetPosition().x;
	//cap.m_Segment.m_Start.y += GetTransform().GetPosition().y;
	//cap.m_Segment.m_Start.z += GetTransform().GetPosition().z;
	//cap.m_Segment.m_End.x += GetTransform().GetPosition().x;
	//cap.m_Segment.m_End.y += GetTransform().GetPosition().y;
	//cap.m_Segment.m_End.z += GetTransform().GetPosition().z;
	XMFLOAT4 se;
	se.x = cap.m_Segment.m_Start.x;
	se.y = cap.m_Segment.m_Start.y;
	se.z = cap.m_Segment.m_Start.z;
	se.w = 1;
	XMStoreFloat4(&se, XMVector4Transform(XMLoadFloat4(&se), XMLoadFloat4x4(&GetTransform().GetWorld())));
	cap.m_Segment.m_Start.x = se.x;
	cap.m_Segment.m_Start.y = se.y;
	cap.m_Segment.m_Start.z = se.z;

	se.x = cap.m_Segment.m_End.x;
	se.y = cap.m_Segment.m_End.y;
	se.z = cap.m_Segment.m_End.z;
	se.w = 1;
	XMStoreFloat4(&se, XMVector4Transform(XMLoadFloat4(&se), XMLoadFloat4x4(&GetTransform().GetWorld())));
	cap.m_Segment.m_End.x = se.x;
	cap.m_Segment.m_End.y = se.y;
	cap.m_Segment.m_End.z = se.z;

	return cap;
}

void CapsuleCollider::Update(float dt)
{

}
void CapsuleCollider::setRadius(float r)
{

}
void CapsuleCollider::SetSegment(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e)
{

}
float CapsuleCollider::GetRadius()
{
	return radius;
}