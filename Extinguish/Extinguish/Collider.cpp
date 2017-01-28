#include "Collider.h"


Collider::Collider(GameObject* o, bool trigger) : Component(o)
{
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


void OnCollisionEnter(Collider* c)
{
	if (c->GetGameObject()->GetName() == "Bob")
	{

	}
}