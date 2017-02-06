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

bool HexagonCollider::CheckSphereAABB(int _min, int _max, Sphere s)
{
	AABB check;
	check.max = poses[_max] + float3(1, 20, 1);
	check.min = poses[_min] - float3(1, 10, 1);
	AABB sph;
	sph.min = s.m_Center - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	sph.max = s.m_Center + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(sph, check);
}

bool HexagonCollider::CheckSphereAABB(AABB test, Sphere s)
{
	AABB cap;
	cap.min = s.m_Center - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	cap.max = s.m_Center + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(test, cap);
}

bool HexagonCollider::CheckCapsuleAABB(int _min, int _max, Capsule s)
{
	AABB check;
	check.max = poses[_max] + float3(1, 20, 1);
	check.min = poses[_min] - float3(1, 10, 1);
	AABB cap;
	cap.min = s.m_Segment.m_Start - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	cap.max = s.m_Segment.m_End + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(check, cap);
}

bool HexagonCollider::CheckCapsuleAABB(AABB test, Capsule s)
{
	AABB cap;
	cap.min = s.m_Segment.m_Start - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	cap.max = s.m_Segment.m_End + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(test, cap);
}

void HexagonCollider::Update(float dt)
{
	vector<GameObject*>* Others = GetGameObject()->GetGameObjects();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = (*Others).size();

	//If Not Floor
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
	//If Floor
	else
	{
		int Top = row * col - 1;
		int TopHalf = (int)(row * 0.75f) * col;
		int Half = (int)(row * 0.5f) * col;
		int BottomHalf = (int)(row * 0.25f) * col;

		for (int f = 0; f < size; ++f)
		{
			SphereCollider* sphere = (*Others)[f]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					Sphere s = sphere->GetWorldSphere();
					float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();

					//Top
					if (CheckSphereAABB((int)(row * 0.5f) * col, row * col - 1, s))
					{
						//TopTop
						if (CheckSphereAABB((int)(row * 0.75f) * col, row * col - 1, s))
						{
							for (int i = (int)(row * 0.75f); i < row; ++i)
							{
								AABB test;
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
										{
											sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
											sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
										}
									}
								}
							}
						}
						//TopBottom
						else if (CheckSphereAABB((int)(row * 0.5f) * col, (int)(row * 0.75f) * col, s))
						{
							for (int i = (int)(row * 0.5f); i < (int)(row * 0.75f); ++i)
							{
								AABB test;
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
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
					//Bottom
					else if (CheckSphereAABB(0, (int)(row * 0.5f) * col + col - 1, s))
					{
						//BottomTop
						if (CheckSphereAABB((int)(row * 0.25f) * col, (int)(row * 0.5f) * col, s))
						{
							for (int i = (int)(row * 0.25f); i < (int)(row * 0.5f); ++i)
							{
								AABB test;
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										if (HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel))
										{
											sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
											sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
										}
									}
								}
							}
						}
						//BottomBottom
						else if (CheckSphereAABB(0, (int)(row * 0.25f) * col, s))
						{
							for (int i = 0; i < (int)(row * 0.25f); ++i)
							{
								AABB test;
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
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
				}
			}

			CapsuleCollider* cap = (*Others)[f]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					Capsule c = cap->GetWorldCapsule();
					float3 vel = cap->GetGameObject()->GetTransform()->GetVelocity();

					//Top
					if (CheckCapsuleAABB((int)(floor(row * 0.5f)) * col, row * col - 1, c))
					{
						//TopTop
						if (CheckCapsuleAABB((int)(floor(row * 0.75f)) * col, row * col - 1, c))
						{
							//TopTopTop
							if (CheckCapsuleAABB((int)floor(row * 0.83f) * col, row * col - 1, c))
							{
								for (int i = (int)floor(row * 0.83f); i < row; ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test,c))
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
							//TopTopBottom
							else if (CheckCapsuleAABB((int)floor(row * 0.75f) * col, (int)ceil(row * 0.83f) * col, c))
							{
								for (int i = (int)floor(row * 0.75f); i < (int)ceil(row * 0.83f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
						//TopBottom
						else if (CheckCapsuleAABB((int)(floor(row * 0.5f)) * col, (int)(ceil(row * 0.75f)) * col, c))
						{
							//TopBottomTop
							if (CheckCapsuleAABB((int)floor(row * 0.63f) * col, (int)ceil(row * 0.75f) * col, c))
							{
								for (int i = (int)floor(row * 0.63f); i < (int)ceil(row * 0.75f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
							//TopBottomBottom
							else if (CheckCapsuleAABB((int)floor(row * 0.5f) * col, (int)ceil(row * 0.63f) * col, c))
							{
								for (int i = (int)floor(row * 0.5f); i < (int)ceil(row * 0.63f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
					//Bottom
					else if (CheckCapsuleAABB(0, (int)(ceil(row * 0.5f)) * col, c))
					{

						//BottomTop
						if (CheckCapsuleAABB((int)(floor(row * 0.25f)) * col, (int)(ceil(row * 0.5f)) * col, c))
						{
							//BottomTopTop
							if (CheckCapsuleAABB((int)floor(row * 0.38f) * col, (int)ceil(row * 0.5f) * col, c))
							{
								for (int i = (int)floor(row * 0.38f); i < (int)ceil(row * 0.5f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
							//BottomTopBottom
							if (CheckCapsuleAABB((int)floor(row * 0.25f) * col, (int)ceil(row * 0.38f) * col, c))
							{
								for (int i = (int)floor(row * 0.25f); i < (int)ceil(row * 0.38f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
						//BottomBottom
						else if (CheckCapsuleAABB(0, (int)(ceil(row * 0.25f)) * col, c))
						{
							//BottomBottomTop
							if (CheckCapsuleAABB((int)floor(row * 0.13f) * col, (int)ceil(row * 0.25f) * col, c))
							{
								for (int i = (int)floor(row * 0.13f); i < (int)ceil(row * 0.25f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
							//BottomBottomBottom
							if (CheckCapsuleAABB(0, (int)ceil(row * 0.13f) * col, c))
							{
								for (int i = 0; i < (int)ceil(row * 0.13f); ++i)
								{
									AABB test;
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
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
		}
	}
}
