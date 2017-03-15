#include "Collider.h"
#include "GameObject.h"

Collider::Collider(GameObject* o, bool trigger) : Component(o), IsTrigger(trigger)
{
	transform = o->GetTransform();
}

Collider::Collider(GameObject* o, bool trigger, ColliderType t) : Component(o), IsTrigger(trigger), type(t)
{

}

bool Collider::isColliding(Collider* c)
{
	for (int i = 0; i < (int)checked.size(); ++i)
	{
		if (checked[i] == c) return true;
	}
	return false;
}


//void OnCollisionEnter(Collider* c)
//{
//	if (c->GetGameObject()->GetName() == "Bob")
//	{
//
//	}
//}