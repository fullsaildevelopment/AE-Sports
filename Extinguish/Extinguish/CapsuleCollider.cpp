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
			if (box->isTrigger() || isTrigger())
			{
				if (AABBToCapsule(box->GetWorldAABB(), GetWorldCapsule()))
				{
					box->GetGameObject()->OnTriggerEnter(this);
					GetGameObject()->OnTriggerEnter(box);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				float3 pos = GetGameObject()->GetTransform()->GetPosition();
				float3 vel = XMtoF(GetGameObject()->GetTransform()->GetVelocity());
				if (AABBToCapsuleReact(box->GetWorldAABB(), c, vel, pos))
				{
					GetGameObject()->GetTransform()->SetPosition(pos);
					GetGameObject()->GetTransform()->SetVelocity(FtoXM(vel));
					printf("CR");
				}
			}
			continue;
		}
		///////////////////////////////////////Capsule vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (isTrigger() || capsule->isTrigger())
			{
				if (CapsuleToCapsule(GetWorldCapsule(), capsule->GetWorldCapsule()))
				{
					capsule->GetGameObject()->OnTriggerEnter(this);
					GetGameObject()->OnTriggerEnter(capsule);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				Capsule o = capsule->GetWorldCapsule();
				float3 pos = GetGameObject()->GetTransform()->GetPosition();
				float3 opos = capsule->GetGameObject()->GetTransform()->GetPosition();
				float3 vel = XMtoF(GetGameObject()->GetTransform()->GetVelocity());
				float3 ovel = XMtoF(capsule->GetGameObject()->GetTransform()->GetVelocity());
				if (SweptCaptoSweptCap(c, o, vel, ovel, pos, opos))
				{
					GetGameObject()->GetTransform()->SetPosition(pos);
					GetGameObject()->GetTransform()->SetVelocity(FtoXM(vel * 0.6f));
					capsule->GetGameObject()->GetTransform()->SetPosition(opos);
					capsule->GetGameObject()->GetTransform()->SetVelocity(FtoXM(ovel * 0.6f));
					capsule->GetGameObject()->OnCollisionEnter(this);
					GetGameObject()->OnCollisionEnter(capsule);
					capsule->checked.push_back(this);
				}
			}
			continue;
		}
		///////////////////////////////////////Capsule vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (isTrigger() || sphere->isTrigger())
			{
				if (CapsuleToSphere(GetWorldCapsule(), sphere->GetWorldSphere()))
				{
					capsule->GetGameObject()->OnTriggerEnter(this);
					GetGameObject()->OnTriggerEnter(sphere);
				}
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