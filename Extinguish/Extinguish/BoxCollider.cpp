#include "BoxCollider.h"
#include "CollisionLibrary.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"

using namespace DirectX;

AABB BoxCollider::GetAABB()
{
	AABB abox;
	abox.min.x = min.x;
	abox.min.y = min.y;
	abox.min.z = min.z;
	abox.max.x = max.x;
	abox.max.y = max.y;
	abox.max.z = max.z;
	return abox;
}
AABB BoxCollider::GetWorldAABB()
{
	AABB box = GetAABB();
	float3 m = GetGameObject()->GetTransform()->GetPosition();
	box.max.x += m.x;
	box.max.y += m.y;
	box.max.z += m.z;
	box.min.x += m.x;
	box.min.y += m.y;
	box.min.z += m.z;
	return box;
}
BoxCollider::BoxCollider(GameObject* g, bool t, XMFLOAT3 _max, XMFLOAT3 _min) : Collider(g, t)
{
	max = _max;
	min = _min;
}

void BoxCollider::Update(float dt)
{
	GameObject* tg = GetGameObject();
	vector<GameObject*>* Others = tg->GetGameObjects();
	size_t size = (*Others).size();

	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == tg) continue;
		bool c = false;
		for (int j = 0; j < checked.size(); ++j)
		{
			if ((*Others)[i]->GetComponent<Collider>() == checked[j]) c = true;
		}
		if (c) continue;
		///////////////////////////////////////AABB vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			GameObject* og = box->GetGameObject();
			if (box->isTrigger() || isTrigger())
			{
				if (AABBtoAABB(GetWorldAABB(), box->GetWorldAABB()))
				{
					if (box->isTrigger() || isTrigger())
					{
						og->OnTriggerEnter(this);
						tg->OnTriggerEnter(box);
					}
				}
			}
			else
			{
				float normx, normy, normz;
				float t = SweptAABBtoAABB(GetWorldAABB(), box->GetWorldAABB(), tg->GetTransform()->GetVelocity() * dt, normx, normy, normz);
				if (t < 1)
				{
					float3 pos = tg->GetTransform()->GetPosition();
					float3 vel = tg->GetTransform()->GetVelocity();
					pos.x += vel.x * dt * t;
					pos.y += vel.y * dt * t;
					pos.z += vel.z * dt * t;
					tg->GetTransform()->SetPosition(pos);
					float rt = 1 - t;
					float3 v = vel * rt;
					float3 norms = float3(normx, normy, normz);
					float3 rejv = v - norms * dot_product(norms, v);
					vel.x = rejv.x;
					vel.y = rejv.y;
					vel.z = rejv.z;
					tg->GetTransform()->SetVelocity(vel);
					tg->OnCollisionEnter(box);
					box->GetGameObject()->OnCollisionEnter(this);
				}


			}
			continue;
		}

		/*
		///////////////////////////////////////AABB vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (capsule->isTrigger() || isTrigger())
			{
				if (AABBToCapsule(GetWorldAABB(), capsule->GetWorldCapsule()))
				{
					tg->OnTriggerEnter(capsule);
					capsule->GetGameObject()->OnTriggerEnter(this);
				}
			}
			continue;
		}
		///////////////////////////////////////AABB vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (sphere->isTrigger() || isTrigger())
			{
				if (SphereToAABB(sphere->GetWorldSphere(), GetWorldAABB()))
				{
					tg->OnTriggerEnter(sphere);
					sphere->GetGameObject()->OnTriggerEnter(this);
				}
			}
		}
		*/

		continue;
	}
}