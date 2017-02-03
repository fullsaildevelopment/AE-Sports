#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "Physics.h"

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
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
					if (HexagonToSphere(*GetWorldHex(), sphere->GetWorldSphere(), vel))
					{
						if (vel.y * dt <= 0.01f && vel.y * dt >= -0.01f)
						{
							vel.y = 0;
							if (vel.isEquil(float3(0, 0, 0)))
								sphere->GetGameObject()->GetComponent<Physics>()->SetIsKinematic(true);
						}
						sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
					}
				}
			}
			CapsuleCollider* cap = (*Others)[i]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();
					Capsule c = cap->GetWorldCapsule();
					if (HexagonToCapsule(*GetWorldHex(), cap->GetWorldCapsule(), vel))
					{
						cap->GetGameObject()->GetTransform()->SetVelocity(vel);
						cap->GetGameObject()->GetTransform()->SetPosition(c.m_Segment.m_Start);
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
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					AABB tophalf;
					tophalf.max = poses[row * col - 1] + float3(0, 20, 0);
					tophalf.min = poses[(int)(row * 0.5f) * col] - float3(0, 10, 0);
					AABB bottomhalf;
					bottomhalf.max = poses[(int)(row * 0.5f) * col + col - 1] + float3(0, 20, 0);
					bottomhalf.min = poses[0] - float3(0, 10, 0);

					if (SphereToAABB(sphere->GetWorldSphere(), tophalf))
					{
						for (int i = (int)(row * 0.5f); i < row; ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
								Sphere s = sphere->GetWorldSphere();
								if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
								{
									sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
									sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
								}
							}
						}
					}
					else if (SphereToAABB(sphere->GetWorldSphere(), bottomhalf))
					{
						for (int i = 0; i < (int)(row * 0.5f); ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
								Sphere s = sphere->GetWorldSphere();
								if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
								{
									sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
									sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
								}
							}
						}
					}
				}
			}
			CapsuleCollider* cap = (*Others)[f]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					AABB tophalf;
					tophalf.max = poses[row * col - 1] + float3(0, 20, 0);
					tophalf.min = poses[(int)(row * 0.5f) * col] - float3(0, 10, 0);
					AABB bottomhalf;
					tophalf.max = poses[(int)(row * 0.5f) * col] + float3(0, 20, 0);
					tophalf.min = poses[0] - float3(0, 10, 0);

					if (AABBToCapsule(tophalf, cap->GetWorldCapsule()))
					{
						Capsule c = cap->GetWorldCapsule();
						float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();
						for (int i = (int)(row * 0.5f); i < row; ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								if (HexagonToCapsule(*GetWorldHex(i * col + j), c, vel))
								{
									cap->GetGameObject()->GetTransform()->SetVelocity(vel);
									cap->GetGameObject()->GetTransform()->SetPosition(c.m_Segment.m_Start);
								}
							}
						}
					}
					else if (AABBToCapsule(tophalf, cap->GetWorldCapsule()))
					{
						float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();
						Capsule c = cap->GetWorldCapsule();
						for (int i = 0; i < (int)(row * 0.5f); ++i)
						{
							for (int j = 0; j < col; ++j)
							{
								if (HexagonToCapsule(*GetWorldHex(i * col + j), c, vel))
								{
									cap->GetGameObject()->GetTransform()->SetVelocity(vel);
									cap->GetGameObject()->GetTransform()->SetPosition(c.m_Segment.m_Start);
								}
							}
						}
					}
				}
			}
		}
	}
}
