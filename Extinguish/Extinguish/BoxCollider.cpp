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

void BoxCollider::Update(float dt)
{
	vector<GameObject*>* Others1 = GetGameObject()->GetGameObjects();
	vector<GameObject*>* Others2 = GetGameObject()->GetGameObjects();
	vector<GameObject*>* Others3 = GetGameObject()->GetGameObjects();
	concurrency::task<void> AABBChecks([Others1, this]()
	{
		size_t size = Others1->size();
		for (int i = 0; i < size; ++i)
		{
			BoxCollider* box = (*Others1)[i]->GetComponent<BoxCollider>();
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
							//box->GetGameObject()->OnTriggerEnter();
						}
					}
				}

			}
		}
	});

	concurrency::task<void> SphereChecks([Others2]()
	{
		size_t size = Others2->size();
		for (int i = 0; i < size; ++i)
		{
			SphereCollider* sphere = (*Others2)[i]->GetComponent<SphereCollider>();
			if (sphere)
			{

			}
		}
	});

	concurrency::task<void> CapsuleChecks([Others3]()
	{
		size_t size = Others3->size();
		for (int i = 0; i < size; ++i)
		{
			CapsuleCollider* capsule = (*Others3)[i]->GetComponent<CapsuleCollider>();
			if (capsule)
			{

			}
		}
	});

}