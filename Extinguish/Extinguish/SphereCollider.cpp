#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"



SphereCollider::SphereCollider(float r, GameObject* o, bool t) : Collider(o,t)
{
	radius = r;
}

void SphereCollider::Update(float dt)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();

	size_t size = (*Others).size();
	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == GetGameObject()) continue;
		///////////////////////////////////////Sphere vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (SphereToAABB(GetWorldSphere(), box->GetWorldAABB()))
			{
				printf("(SvB)");
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
		///////////////////////////////////////Sphere vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (CapsuleToSphere(capsule->GetWorldCapsule(), GetWorldSphere()))
			{
				printf("(SvC)");
				if (!capsule->isTrigger() && !isTrigger())
				{
					//Physics
				}
				else
				{
					if (capsule->isTrigger())
					{

						capsule->GetGameObject()->OnTriggerEnter(this);
					}
					if (isTrigger())
					{
						GetGameObject()->OnTriggerEnter(box);
					}
				}
			}
			continue;
		}
		///////////////////////////////////////Sphere vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (SphereToSphere(sphere->GetWorldSphere(), GetWorldSphere()))
			{
				printf("(SvS)");
				if (!sphere->isTrigger() && !isTrigger())
				{
					//Physics
				}
				else
				{
					if (sphere->isTrigger())
					{

						sphere->GetGameObject()->OnTriggerEnter(this);
					}
					if (isTrigger())
					{
						GetGameObject()->OnTriggerEnter(box);
					}
				}
			}
		}
		continue;
	}
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
	XMFLOAT3 pos = GetGameObject()->GetTransform()->GetPosition();
	Sphere s;
	s.m_Center = float3(pos.x, pos.y, pos.z);
	s.m_Radius = radius;
	return s;
}