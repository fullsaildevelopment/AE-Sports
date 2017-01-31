#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

HexagonCollider::HexagonCollider(GameObject* o, float d, float _height) : Collider(o,false)
{
	poses = nullptr;
	hex.d = d;
	hex.s = d * 0.5f;
	hex.seg.m_Start = o->GetTransform()->GetPosition();
	hex.seg.m_End = o->GetTransform()->GetPosition() + float3(0,_height,0);
	height = _height;
	hex.h = sqrtf(3)*hex.s;
	instanced = 0;
}

HexagonCollider::HexagonCollider(int _instanced, float3* positions, float _height, float d, GameObject* o) : Collider(o, false)
{
	poses = positions;
	height = _height;
	hex.d = d;
	hex.s = d * 0.5f;
	hex.h = sqrtf(3)*hex.s;
	instanced = _instanced;
}

Hexagon* HexagonCollider::GetWorldHex()
{
	hex.seg.m_Start = GetGameObject()->GetTransform()->GetPosition();
	hex.seg.m_End = GetGameObject()->GetTransform()->GetPosition() + float3(0, height, 0);
	return &hex;
}

Hexagon* HexagonCollider::GetWorldHex(int i)
{
	hex.seg.m_Start = poses[i];
	hex.seg.m_End = poses[i] + float3(0, height, 0);
	return &hex;
}


void HexagonCollider::Update(float dt)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = (*Others).size();
	if (!poses)
	{
		for (int i = 0; i < size; ++i)
		{
			SphereCollider* sphere = (*Others)[i]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger())
				{
					float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
					if (HexagonToSphere(*GetWorldHex(), sphere->GetWorldSphere(), vel))
					{
						sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < instanced; ++i)
		{
			for (int f = 0; i < size; ++f)
			{
				SphereCollider* sphere = (*Others)[f]->GetComponent<SphereCollider>();
				if (sphere)
				{
					if (!sphere->isTrigger())
					{
						float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
						if (HexagonToSphere(*GetWorldHex(i), sphere->GetWorldSphere(), vel))
						{
							sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
						}
					}
				}
			}
		}
	}
}
