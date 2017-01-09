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
	XMFLOAT3 m = GetTransform().GetPosition();
	box.max.x += m.x;
	box.max.y += m.y;
	box.max.z += m.z;
	box.min.x += m.x;
	box.min.y += m.y;
	box.min.z += m.z;
	return box;
}
BoxCollider::BoxCollider(GameObject* g, bool t, XMFLOAT3 _max, XMFLOAT3 _min) : Collider(g,t)
{
	max = _max;
	min = _min;
}

void BoxCollider::Update(float dt)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();

	size_t size = Others->size();
	for (int i = 0; i < size; ++i)
	{
		if ((*Others)[i] == GetGameObject()) continue;
		///////////////////////////////////////AABB vs AABB///////////////////////////////////////
		BoxCollider* box = (*Others)[i]->GetComponent<BoxCollider>();
		if (box)
		{
			if (AABBtoAABB(GetWorldAABB(), box->GetWorldAABB()))
			{
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
		}
		///////////////////////////////////////AABB vs Capsule///////////////////////////////////////
		CapsuleCollider* capsule = (*Others)[i]->GetComponent<CapsuleCollider>();
		if (capsule)
		{
			if (AABBToCapsule(GetWorldAABB(), capsule->GetWorldCapsule()))
			{

			}
		}
		///////////////////////////////////////AABB vs Sphere///////////////////////////////////////
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (SphereToAABB(sphere->GetWorldSphere(), GetWorldAABB()))
			{

			}
		}
	}
}