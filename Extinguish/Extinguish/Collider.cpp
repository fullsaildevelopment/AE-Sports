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

void Collider::IgnoreGameObject(GameObject* object)
{
	bool placed = false;
	for (int i = 0; i < ignore.size(); ++i)
	{
		if (ignore[i] == nullptr)
		{
			ignore[i] = object;
			placed = true;
		}
	}
	if (!placed)
	{
		size_t size = ignore.size();
		ignore.resize(size + 4);
		ignore[size] = object;
	}
}

void Collider::UnIgnoreGameObject(GameObject* object)
{
	for (int i = 0; i < ignore.size(); ++i)
	{
		if (ignore[i] == object)
			ignore[i] = nullptr;
	}
}


//void OnCollisionEnter(Collider* c)
//{
//	if (c->GetGameObject()->GetName() == "Bob")
//	{
//
//	}
//}