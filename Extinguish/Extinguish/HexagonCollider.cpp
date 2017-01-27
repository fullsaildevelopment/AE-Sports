#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

HexagonCollider::HexagonCollider(GameObject* o, float d, float _height) : Collider(o,false)
{
	hex.d = d;
	hex.s = d * 0.5f;
	hex.seg.m_Start = o->GetTransform()->GetPosition();
	hex.seg.m_End = o->GetTransform()->GetPosition() + float3(0,_height,0);
	height = _height;
	hex.h = sqrtf(3)*hex.s;
}

Hexagon HexagonCollider::GetWorldHex()
{
	hex.seg.m_Start = GetGameObject()->GetTransform()->GetPosition();
	hex.seg.m_End = GetGameObject()->GetTransform()->GetPosition() + float3(0, height, 0);
	return hex;
}


void HexagonCollider::Update(float dt, InputManager* input)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = (*Others).size();
	for (int i = 0; i < size; ++i)
	{
		SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
		if (sphere)
		{
			if (!sphere->isTrigger())
			{
				float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
				if (HexagonToSphere(GetWorldHex(), sphere->GetWorldSphere(), vel))
				{
					sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
				}
			}
		}
	}
}
