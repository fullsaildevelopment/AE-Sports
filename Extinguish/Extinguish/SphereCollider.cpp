#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"



SphereCollider::SphereCollider(float r, GameObject* o, bool t) : Collider(o, t)
{
	radius = r;
}

SphereCollider::SphereCollider(float r, GameObject* o, bool t, float3 v) : Collider(o, t)
{
	radius = r;
	GetGameObject()->GetTransform()->AddVelocity(v * 3);
}

void SphereCollider::Update(float dt, InputManager* input)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();
	//GetGameObject()->GetTransform()->AddVelocity({ 0, -9.8f * dt, 0 });

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

		///////////////////////////////////////Sphere vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (box->isTrigger() || isTrigger())
			{
				if (SphereToAABB(GetWorldSphere(), box->GetWorldAABB()))
				{
					if (box->isTrigger() || isTrigger())
					{
						box->GetGameObject()->OnTriggerEnter(this);
						GetGameObject()->OnTriggerEnter(box);
					}
				}
			}
			else
			{
				Sphere s = GetWorldSphere();
				float3 vel = GetGameObject()->GetTransform()->GetVelocity() * dt;
				float3 bvel = vel;
				float3 c = SweptSpheretoAABB(s, box->GetWorldAABB(), vel);
				if (c.x == 1 || c.y == 1)
				{
					GetGameObject()->GetTransform()->SetPosition(s.m_Center);
					vel *= c;
					GetGameObject()->GetTransform()->SetVelocity(vel / dt);
					box->GetGameObject()->OnCollisionEnter(this);
					GetGameObject()->OnCollisionEnter(box);
				}
				else if (!bvel.isEquil(vel))
					GetGameObject()->GetTransform()->SetVelocity(vel / dt);
			}
			continue;
		}
		///////////////////////////////////////Sphere vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (capsule->isTrigger() || isTrigger())
			{
				if (CapsuleToSphere(capsule->GetWorldCapsule(), GetWorldSphere()))
				{
					capsule->GetGameObject()->OnTriggerEnter(this);

					GetGameObject()->OnTriggerEnter(capsule);
				}
			}
			else
			{

			}
			continue;
		}
		///////////////////////////////////////Sphere vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (sphere->isTrigger() && isTrigger())
			{
				if (SphereToSphere(sphere->GetWorldSphere(), GetWorldSphere()))
				{
					if (sphere->isTrigger() || isTrigger())
					{
						sphere->GetGameObject()->OnTriggerEnter(this);
						GetGameObject()->OnTriggerEnter(sphere);
					}
				}
			}
			else
			{
				Sphere s = GetWorldSphere();
				Sphere os = sphere->GetWorldSphere();
				float3 vel = GetGameObject()->GetTransform()->GetVelocity() * dt;
				float3 bvel = GetGameObject()->GetTransform()->GetVelocity() * dt;
				float3 svel = sphere->GetGameObject()->GetTransform()->GetVelocity() * dt;
				float3 bsvel = sphere->GetGameObject()->GetTransform()->GetVelocity() * dt;
				if (!vel.isEquil(float3(0, 0, 0)) && svel.isEquil(float3().make_zero()))
				{
					float3 n = SweptSpheretoSphere(s, os, vel);
					if (!n.isEquil(float3(0, 0, 0)))
					{
						GetGameObject()->GetTransform()->SetVelocity(vel / dt);
						GetGameObject()->GetTransform()->SetPosition(s.m_Center);
					}
				}
				else if (!vel.isEquil(float3(0, 0, 0)) && !svel.isEquil(float3().make_zero()))
				{
					if (SweptSpheretoSweptSphere(s, os, vel, svel))
					{
						checked.push_back(sphere);
						sphere->checked.push_back((Collider*)this);
						GetGameObject()->GetTransform()->SetVelocity(vel / dt);
						GetGameObject()->GetTransform()->SetPosition(s.m_Center);
						sphere->GetGameObject()->GetTransform()->SetVelocity(svel / dt);
						sphere->GetGameObject()->GetTransform()->SetPosition(os.m_Center);
						sphere->GetGameObject()->OnCollisionEnter(this);
						GetGameObject()->OnCollisionEnter(sphere);
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
	float3 pos = GetGameObject()->GetTransform()->GetPosition();
	Sphere s;
	s.m_Center = float3(pos.x, pos.y, pos.z);
	s.m_Radius = radius;
	return s;
}