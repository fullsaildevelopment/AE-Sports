#include "Collider.h"


Collider::Collider(GameObject* o, bool trigger) : Component(o)
{
	SetCompType(Component::CTBoxCollider);
	Type = ColliderType::aabb;
	o->GetTransform()->AddChild(&transform);
	IsTrigger = trigger;
}

bool Collider::isColliding(Collider* c)
{
	for (int i = 0; i < (int)checked.size(); ++i)
	{
		if (checked[i] == c) return true;
	}
	return false;
}