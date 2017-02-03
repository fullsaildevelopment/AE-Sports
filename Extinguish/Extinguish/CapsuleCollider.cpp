#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

using namespace DirectX;

CapsuleCollider::CapsuleCollider(float r, XMFLOAT3 s, XMFLOAT3 e, GameObject* o, bool t) : Collider(o, t)
{
	radius = r;
	Start = s;
	End = e;
	otherCapsule = nullptr;
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

void CapsuleCollider::Update(float dt)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = (*Others).size();
	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == GetGameObject()) continue;
		bool c = false;
		for (int j = 0; j < checked.size(); ++j)
		{
			if ((*Others)[i]->GetComponent<Collider>() == checked[j]) c = true;
		}
		if (c) continue;
		///////////////////////////////////////Capsule vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (box->isTrigger() || isTrigger())
			{
				if (AABBToCapsule(box->GetWorldAABB(), GetWorldCapsule()))
				{
					box->GetGameObject()->OnTriggerEnter(this);
					tg->OnTriggerEnter(box);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				float3 pos = tgt->GetPosition();
				float3 vel = tgt->GetVelocity();
				if (AABBToCapsuleReact(box->GetWorldAABB(), c, vel, pos))
				{
					tgt->SetPosition(pos);
					tgt->SetVelocity(vel);
					tg->OnCollisionEnter(box);
					box->GetGameObject()->OnCollisionEnter(this);
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
					tg->OnTriggerEnter(capsule);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				Capsule o = capsule->GetWorldCapsule();
				float3 pos = tgt->GetPosition();
				float3 opos = capsule->GetGameObject()->GetTransform()->GetPosition();
				float3 vel = tgt->GetVelocity();
				float3 ovel = capsule->GetGameObject()->GetTransform()->GetVelocity();
				if (SweptCaptoSweptCap(c, o, vel, ovel, pos, opos))
				{
					tgt->SetPosition(pos);
					tgt->SetVelocity(vel * 0.6f);
					capsule->GetGameObject()->GetTransform()->SetPosition(opos);
					//TODO: Turned off because I need to attack
					//capsule->GetGameObject()->GetTransform()->SetVelocity(ovel * 0.6f);
					capsule->GetGameObject()->OnCollisionEnter(this);
					tg->OnCollisionEnter(capsule);
					capsule->checked.push_back(this);
					otherCapsule = capsule;
				}
				else
				{
					if (otherCapsule == capsule)
					{
						tg->OnCollisionExit(otherCapsule);
						otherCapsule = nullptr;
					}
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
					sphere->GetGameObject()->OnTriggerEnter(this);
					tg->OnTriggerEnter(sphere);
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