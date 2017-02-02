#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

HexagonCollider::HexagonCollider(GameObject* o, float d, float _height) : Collider(o, false)
{
	poses = nullptr;
	hex.d = d;
	hex.s = d * 0.5f;
	hex.seg.m_Start = o->GetTransform()->GetPosition();
	hex.seg.m_End = o->GetTransform()->GetPosition() + float3(0, _height, 0);
	height = _height;
	hex.h = sqrtf(3)*hex.s;
	row = 0;
	col = 0;
}

HexagonCollider::HexagonCollider(int _row, int _col, float3* positions, float _height, float d, GameObject* o) : Collider(o, false)
{
	poses = positions;
	height = _height;
	hex.d = d;
	hex.s = d * 0.5f;
	hex.h = sqrtf(3)*hex.s;
	row = _row;
	col = _col;
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
		for (int f = 0; f < size; ++f)
		{
			SphereCollider* sphere = (*Others)[f]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger())
				{
					AABB tophalf;
					tophalf.max = poses[row * col - 1] + float3(0, 10, 0);
					tophalf.min = poses[(int)(row * 0.5f) * col] - float3(0, 10, 0);
					if (SphereToAABB(sphere->GetWorldSphere(), tophalf))
					{
						for (int i = (int)(row * 0.5f); i < row; ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
								if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
								{
									sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
								}
							}
						}
					}
					else
					{
						for (int i = 0; i < (int)(row * 0.5f); ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
								if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
								{
									sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
								}
							}
						}
					}
				}
			}
			CapsuleCollider* cap = (*Others)[f]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger())
				{
					AABB tophalf;
					tophalf.max = poses[row * col - 1] + float3(0, 10, 0);
					tophalf.min = poses[(int)(row * 0.5f) * col] - float3(0, 10, 0);
					if (AABBToCapsule(tophalf, cap->GetWorldCapsule()))
					{
						for (int i = (int)(row * 0.5f); i < row; ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();
								if (HexagonToCapsule(*GetWorldHex(i * col + j), cap->GetWorldCapsule(), vel))
								{
									cap->GetGameObject()->GetTransform()->SetVelocity(vel);
								}
							}
						}
					}
					else
					{
						for (int i = 0; i < (int)(row * 0.5f); ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();
								if (HexagonToCapsule(*GetWorldHex(i * col + j), cap->GetWorldCapsule(), vel))
								{
									cap->GetGameObject()->GetTransform()->SetVelocity(vel);
								}
							}
						}
					}
			}
		}
	}
}
