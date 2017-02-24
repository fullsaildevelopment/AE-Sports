#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"

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
	float3 pos = GetGameObject()->GetTransform()->GetPosition();
	cap.m_Segment.m_Start.x += pos.x;
	cap.m_Segment.m_Start.y += pos.y;
	cap.m_Segment.m_Start.z += pos.z;
	cap.m_Segment.m_End.x += pos.x;
	cap.m_Segment.m_End.y += pos.y;
	cap.m_Segment.m_End.z += pos.z;

	return cap;
}

void CapsuleCollider::FixedUpdate(float dt)
{
	if (objects.size() == 0)
	{
		objects = *GetGameObject()->GetGameObjects();
		CollidingWith.resize(objects.size());
	}
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = objects.size();
	for (int i = 0; i < size; ++i)
	{
		if (objects[i] == GetGameObject()) continue;
		bool c = false;
		for (int j = 0; j < checked.size(); ++j)
		{
			if (objects[i]->GetComponent<Collider>() == checked[j]) c = true;
		}
		if (c) continue;

		///////////////////////////////////////Capsule vs AABB///////////////////////////////////////
		BoxCollider* box = objects[i]->GetComponent<BoxCollider>();
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
				float3 opos = tgt->GetPosition();
				float3 vel = tgt->GetVelocity();
				float3 result = AABBToCapsuleReact(box->GetWorldAABB(), c, vel, pos - GetCapsule().m_Segment.m_Start);
				if (!result.isEquil(float3(0, 0, 0)))
				{
					bool aG = false;
					if (!result.isEquil(float3(0, 1, 0)))
						aG = true;
					float3 invN = result.negate();
					invN = invN * (vel * result).magnitude();
					vel = vel - invN;

					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel, pos - GetCapsule().m_Segment.m_Start, false, float3(0, 0, 0), aG);
						if (!CollidingWith[i])
						{
							tg->OnCollisionEnter(box);
							objects[i]->OnCollisionEnter(this);
							CollidingWith[i] = true;
						}
						continue;
					}
				}
					if (CollidingWith[i])
					{
						CollidingWith[i] = false;
						objects[i]->OnCollisionExit(this);
						tg->OnCollisionExit(box);
					}
				else if (otherBox == box)
				{
					otherBox->OnCollisionExit(this);
					tg->OnCollisionExit(otherBox);
					otherBox = nullptr;
				}
			}
			continue;
		}
		///////////////////////////////////////Capsule vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = objects[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (isTrigger() || capsule->isTrigger())
			{
				if (CapsuleToCapsule(GetWorldCapsule(), capsule->GetWorldCapsule()))
				{
					objects[i]->OnTriggerEnter(this);
					tg->OnTriggerEnter(capsule);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				Capsule o = capsule->GetWorldCapsule();
				float3 pos = tgt->GetPosition();
				float3 opos = objects[i]->GetTransform()->GetPosition();
				float3 vel = tgt->GetVelocity();
				float3 ovel = objects[i]->GetTransform()->GetVelocity();
				if (SweptCaptoSweptCap(c, o, vel, ovel, pos, opos))
				{
					checked.push_back(capsule);
					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel, pos - GetCapsule().m_Segment.m_Start, false, float3(0, 0, 0), true);
						Physics* nop = objects[i]->GetComponent<Physics>();
						if (nop)
						{
							nop->HandlePhysics(objects[i]->GetTransform(), ovel, opos - capsule->GetCapsule().m_Segment.m_Start,false,float3(0,0,0),true);
						}
					}
					if (!CollidingWith[i])
					{
						objects[i]->OnCollisionEnter(this);
						tg->OnCollisionEnter(capsule);
						CollidingWith[i] = true;
					}
					continue;
				}
				if (CollidingWith[i])
				{
					CollidingWith[i] = false;
					objects[i]->OnCollisionExit(this);
					tg->OnCollisionExit(capsule);
				}
			}
			continue;
		}
		/*
		///////////////////////////////////////Capsule vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (sphere->isEnabled())
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
		}
		*/
	}
	checked.clear();
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