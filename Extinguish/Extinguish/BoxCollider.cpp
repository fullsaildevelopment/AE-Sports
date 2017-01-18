#include "BoxCollider.h"
#include "CollisionLibrary.h"
#include <ppltasks.h>
#include "SphereCollider.h"
#include "CapsuleCollider.h"

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

void BoxCollider::Update(float dt, InputManager* input)
{
	GameObject* tg = GetGameObject();
	vector<GameObject*>* Others = tg->GetGameObjects();
	size_t size = (*Others).size();
	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == tg) continue;
		///////////////////////////////////////AABB vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			GameObject* og = box->GetGameObject();
			if (box->isTrigger() || isTrigger())
			{
				if (AABBtoAABB(GetWorldAABB(), box->GetWorldAABB()))
				{
					if (box->isTrigger())
					{
						if (og->OnTriggerEnter)
							og->OnTriggerEnter(this,box);
					}
					if (isTrigger())
					{
						if (tg->OnTriggerEnter)
							tg->OnTriggerEnter(box,this);
					}
				}
			}
			else
			{
				float normx, normy, normz;
				float t = SweptAABBtoAABB(GetWorldAABB(), box->GetWorldAABB(), XMtoF(tg->GetTransform()->GetVelocity()) * dt, normx, normy, normz);
				if (t < 1)
				{
					float3 pos = tg->GetTransform()->GetPosition();
					XMFLOAT3 vel = tg->GetTransform()->GetVelocity();
					pos.x += vel.x * dt * t;
					pos.y += vel.y * dt * t;
					pos.z += vel.z * dt * t;
					tg->GetTransform()->SetPosition(pos);
					float rt = 1 - t;
					float3 v = XMtoF(vel) * rt;
					float3 norms = float3(normx, normy, normz);
					float3 rejv = v - norms * dot_product(norms, v);
					vel.x = rejv.x;
					vel.y = rejv.y;
					vel.z = rejv.z;
					tg->GetTransform()->SetVelocity(vel);
				}


			}
			continue;
		}
		///////////////////////////////////////AABB vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (AABBToCapsule(GetWorldAABB(), capsule->GetWorldCapsule()))
			{
				printf("C");

			}
			continue;
		}
		///////////////////////////////////////AABB vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (SphereToAABB(sphere->GetWorldSphere(), GetWorldAABB()))
			{
				//printf("S");

			}
		}
		continue;
	}
}