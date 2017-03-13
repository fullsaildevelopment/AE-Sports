#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "GameObject.h"



SphereCollider::SphereCollider(float r, GameObject* o, bool t) : Collider(o, t)
{
	radius = r;
}

SphereCollider::SphereCollider(float r, GameObject* o, bool t, float3 v) : Collider(o, t)
{
	radius = r;
	GetGameObject()->GetTransform()->AddVelocity(v * 3);
}

void SphereCollider::FixedUpdate(float _dt)
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

		///////////////////////////////////////Sphere vs AABB///////////////////////////////////////
		BoxCollider* box = objects[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (box->isTrigger() || isTrigger())
			{
				if (SphereToAABB(GetWorldSphere(), box->GetWorldAABB()))
				{
					if (box->isTrigger() || isTrigger())
					{
						box->GetGameObject()->OnTriggerEnter(this);
						tg->OnTriggerEnter(box);
					}
				}
			}
			else
			{
				Sphere s = GetWorldSphere();
				float3 vel = tgt->GetVelocity() * _dt;
				float3 c = SweptSpheretoAABB(s, box->GetWorldAABB(), vel);
				if (c.x == 1 || c.y == 1)
				{
					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel * c / _dt, s.m_Center, true);
						if (!CollidingWith[i])
						{
							objects[i]->OnCollisionEnter(this);
							tg->OnCollisionEnter(box);
							CollidingWith[i] = true;
						}
						continue;
					}
					else
					{
						tgt->SetPosition(s.m_Center);
						vel *= c;
						tgt->SetVelocity(vel / _dt);
						if (!CollidingWith[i])
						{
							objects[i]->OnCollisionEnter(this);
							tg->OnCollisionEnter(box);
							CollidingWith[i] = true;
						}
					}
					if (CollidingWith[i])
					{
						CollidingWith[i] = false;
						objects[i]->OnCollisionExit(this);
						tg->OnCollisionExit(box);
					}
				}
			}
			continue;
		}
		///////////////////////////////////////Sphere vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = objects[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (capsule->isTrigger() || isTrigger())
			{
				if (CapsuleToSphere(capsule->GetWorldCapsule(), GetWorldSphere()))
				{
					capsule->GetGameObject()->OnTriggerEnter(this);

					tg->OnTriggerEnter(capsule);
				}
			}
			else
			{
				Sphere s = GetWorldSphere();
				float3 vel = tgt->GetVelocity();
				if (CapsuleToSphereReact(capsule->GetWorldCapsule(), s, vel))
				{
					Physics* op = tg->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(tgt, vel, s.m_Center, true);
						if (!CollidingWith[i])
						{
							objects[i]->OnCollisionEnter(this);
							tg->OnCollisionEnter(capsule);
							CollidingWith[i] = true;
							continue;
						}
					}
					else
					{
						tgt->SetPosition(s.m_Center);
						tgt->SetVelocity(vel);
						capsule->GetGameObject()->OnCollisionEnter(this);
						tg->OnCollisionEnter(capsule);
					}
					if (CollidingWith[i])
					{
						CollidingWith[i] = false;
						objects[i]->OnCollisionExit(this);
						tg->OnCollisionExit(capsule);
					}
				}

			}
			continue;
		}
		///////////////////////////////////////Sphere vs Sphere///////////////////////////////////////
		SphereCollider* sphere = objects[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (sphere->isTrigger() || isTrigger())
			{
				if (SphereToSphere(sphere->GetWorldSphere(), GetWorldSphere()))
				{

					sphere->GetGameObject()->OnTriggerEnter(this);
					tg->OnTriggerEnter(sphere);
				}
			}

			else
			{
				Sphere s = GetWorldSphere();
				Sphere os = sphere->GetWorldSphere();
				float3 vel = tgt->GetVelocity() * _dt;
				float3 bvel = tgt->GetVelocity() * _dt;
				float3 svel = sphere->GetGameObject()->GetTransform()->GetVelocity() * _dt;
				float3 bsvel = sphere->GetGameObject()->GetTransform()->GetVelocity() * _dt;
				if (!vel.isEquil(float3(0, 0, 0)) && svel.isEquil(float3().make_zero()))
				{
					float3 n = SweptSpheretoSphere(s, os, vel);
					if (!n.isEquil(float3(0, 0, 0)))
					{
						tgt->SetVelocity(vel / _dt);
						tgt->SetPosition(s.m_Center);
						if (!CollidingWith[i])
						{
							objects[i]->OnCollisionEnter(this);
							tg->OnCollisionEnter(sphere);
							CollidingWith[i] = true;
						}
						continue;
					}
					if (CollidingWith[i])
					{
						CollidingWith[i] = false;
						objects[i]->OnCollisionExit(this);
						tg->OnCollisionExit(sphere);
					}
				}
				else if (!vel.isEquil(float3(0, 0, 0)) && !svel.isEquil(float3().make_zero()))
				{
					if (SweptSpheretoSweptSphere(s, os, vel, svel))
					{
						checked.push_back(sphere);
						sphere->checked.push_back((Collider*)this);
						tgt->SetVelocity(vel / _dt);
						tgt->SetPosition(s.m_Center);
						objects[i]->GetTransform()->SetVelocity(svel / _dt);
						objects[i]->GetTransform()->SetPosition(os.m_Center);
						objects[i]->OnCollisionEnter(this);
						tg->OnCollisionEnter(sphere);
					}
				}
			}
			continue;
		}
	}
	checked.clear();
}
Sphere SphereCollider::GetSphere()
{
	Sphere s;
	s.m_Center = float3(0, 0, 0);
	s.m_Radius = radius;
	return s;
}
Sphere SphereCollider::GetWorldSphere()
{
	XMFLOAT4X4 m = *GetGameObject()->GetTransform()->GetWorldP();
	Sphere s;
	s.m_Center = float3(m._41, m._42, m._43);
	s.m_Radius = radius;
	return s;
}