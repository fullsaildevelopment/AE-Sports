#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"

using namespace DirectX;

CapsuleCollider::CapsuleCollider(float r, XMFLOAT3 s, XMFLOAT3 e, GameObject* o, bool t) : Collider(o, t, CTCapsule)
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
	if (!transform)
	{
		transform = GetGameObject()->GetTransform();
	}
	Capsule cap;
	cap.m_Radius = radius;
	cap.m_Segment.m_Start.x = Start.x;
	cap.m_Segment.m_Start.y = Start.y;
	cap.m_Segment.m_Start.z = Start.z;
	cap.m_Segment.m_End.x = End.x;
	cap.m_Segment.m_End.y = End.y;
	cap.m_Segment.m_End.z = End.z;

	float3 pos = transform->GetPosition();
	cap.m_Segment.m_Start.x += pos.x;
	cap.m_Segment.m_Start.y += pos.y;
	cap.m_Segment.m_Start.z += pos.z;
	cap.m_Segment.m_End.x += pos.x;
	cap.m_Segment.m_End.y += pos.y;
	cap.m_Segment.m_End.z += pos.z;

	return cap;
}

void CapsuleCollider::FixedUpdate(float _dt)
{
	collisionNormal = { 0,0,0 };
	if (objects.size() == 0)
	{
		objects = *GetGameObject()->GetGameObjects();
		CollidingWith.resize(objects.size());
	}
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = objects.size();
	GameObject* ob;
	for (int i = 0; i < size; ++i)
	{
		ob = objects[i];
		if (ob == GetGameObject()) continue;
		bool c = false;
		for (int j = 0; j < checked.size(); ++j)
		{
			Collider* ch = objects[i]->GetComponent<Collider>();
			if (ob->GetComponent<Collider>() == checked[j])
				c = true;
		}
		if (c) continue;

		///////////////////////////////////////Capsule vs AABB///////////////////////////////////////
		BoxCollider* box = ob->GetComponent<BoxCollider>();
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
				float3 result = AABBToCapsuleReact(box->GetWorldAABB(), c, vel, pos);
				if (!result.isEquil(float3(0, 0, 0)))
				{
					box->collisionNormal = result;
					collisionNormal = result;
					bool aG = false;

					if (!result.isEquil(float3(0, 1, 0)))
						aG = true;

					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel, pos - GetCapsule().m_Segment.m_Start, false, result, aG, true);
						if (!CollidingWith[i])
						{
							tg->OnCollisionEnter(box);
							ob->OnCollisionEnter(this);
							CollidingWith[i] = true;
						}
						else
						{
							tg->OnCollisionStay(box);
							ob->OnCollisionStay(this);
						}
						continue;
					}
				}
				if (CollidingWith[i])
				{
					CollidingWith[i] = false;
					ob->OnCollisionExit(this);
					tg->OnCollisionExit(box);
				}
			}
			continue;
		}
		///////////////////////////////////////Capsule vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = ob->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (isTrigger() || capsule->isTrigger())
			{
				if (CapsuleToCapsule(GetWorldCapsule(), capsule->GetWorldCapsule()))
				{
					ob->OnTriggerEnter(this);
					tg->OnTriggerEnter(capsule);
				}
			}
			else
			{
				Capsule c = GetWorldCapsule();
				Capsule o = capsule->GetWorldCapsule();
				float3 pos = tgt->GetPosition();
				float3 opos = ob->GetTransform()->GetPosition();
				float3 vel = tgt->GetVelocity();
				float3 ovel = ob->GetTransform()->GetVelocity();
				if (SweptCaptoSweptCap(c, o, vel, ovel, pos, opos))
				{
					capsule->checked.push_back(this);
					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel, pos - GetCapsule().m_Segment.m_Start, false, float3(0, 0, 0), true);
						Physics* nop = ob->GetComponent<Physics>();
						if (nop)
						{
							nop->HandlePhysics(ob->GetTransform(), ovel, opos - capsule->GetCapsule().m_Segment.m_Start, false, float3(0, 0, 0), true);
						}
					}
					if (!CollidingWith[i])
					{
						ob->OnCollisionEnter(this);
						tg->OnCollisionEnter(capsule);
						CollidingWith[i] = true;
					}
					else
					{
						tg->OnCollisionStay(capsule);
						ob->OnCollisionStay(this);
					}
					continue;
				}
				if (CollidingWith[i])
				{
					CollidingWith[i] = false;
					ob->OnCollisionExit(this);
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