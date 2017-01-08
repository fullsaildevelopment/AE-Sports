#include "BoxCollider.h"
#include "CollisionLibrary.h"
#include <ppltasks.h>

void BoxCollider::Update(float dt)
{
	vector<GameObject*> Others = *(GetGameObject()->GetGameObjects());

	concurrency::task<void> AABBChecks([]()
	{

	});

	concurrency::task<void> SphereChecks([]()
	{

	});

	concurrency::task<void> CapsuleChecks([]()
	{

	});

}