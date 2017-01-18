#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

using namespace DirectX;

CapsuleCollider::CapsuleCollider(float r, XMFLOAT3 s, XMFLOAT3 e, GameObject* o, bool t) : Collider(o, t)
{
	radius = r;
	Start = s;
	End = e;
}


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
	//XMFLOAT4 se;
	//se.x = cap.m_Segment.m_Start.x;
	//se.y = cap.m_Segment.m_Start.y;
	//se.z = cap.m_Segment.m_Start.z;
	//se.w = 1;
	//XMStoreFloat4(&se, XMVector4Transform(XMLoadFloat4(&se), XMLoadFloat4x4(&GetTransform().GetWorld())));
	//cap.m_Segment.m_Start.x = se.x;
	//cap.m_Segment.m_Start.y = se.y;
	//cap.m_Segment.m_Start.z = se.z;
	//
	//se.x = cap.m_Segment.m_End.x;
	//se.y = cap.m_Segment.m_End.y;
	//se.z = cap.m_Segment.m_End.z;
	//se.w = 1;
	//XMStoreFloat4(&se, XMVector4Transform(XMLoadFloat4(&se), XMLoadFloat4x4(&GetTransform().GetWorld())));
	//cap.m_Segment.m_End.x = se.x;
	//cap.m_Segment.m_End.y = se.y;
	//cap.m_Segment.m_End.z = se.z;
	float3 pos = GetGameObject()->GetTransform()->GetPosition();
	cap.m_Segment.m_Start.x += pos.x;
	cap.m_Segment.m_Start.y += pos.y;
	cap.m_Segment.m_Start.z += pos.z;
	cap.m_Segment.m_End.x += pos.x;
	cap.m_Segment.m_End.y += pos.y;
	cap.m_Segment.m_End.z += pos.z;

	return cap;
}

void CapsuleCollider::Update(float dt, InputManager* input)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();

	size_t size = (*Others).size();
	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == GetGameObject()) continue;
		///////////////////////////////////////Capsule vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (AABBToCapsule(box->GetWorldAABB(), GetWorldCapsule()))
			{
				printf("(CvB)");
				if (!box->isTrigger() && !isTrigger())
				{
					//Physics
				}
				else
				{
					if (box->isTrigger())
					{

						box->GetGameObject()->OnTriggerEnter(this);
					}
					if (isTrigger())
					{
						GetGameObject()->OnTriggerEnter(box);
					}
				}
			}
			continue;
		}
		///////////////////////////////////////Capsule vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (CapsuleToCapsule(GetWorldCapsule(), capsule->GetWorldCapsule()))
			{
				printf("(CvC)");

			}
			continue;
		}
		///////////////////////////////////////Capsule vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (CapsuleToSphere(GetWorldCapsule(), sphere->GetWorldSphere()))
			{
				printf("(CvS)");

			}
		}
		continue;
	}
}
void CapsuleCollider::setRadius(float r)
{
	radius = r;
}
void CapsuleCollider::SetSegment(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e)
{
	Start = s;
	End = e;
}
float CapsuleCollider::GetRadius()
{
	return radius;
}