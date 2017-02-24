#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "Physics.h"
#include "GameObject.h"

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
void HexagonCollider::FixedUpdate(float dt)
{
	if (objects.size() == 0)
	{
		objects = *GetGameObject()->GetGameObjects();
		CollidingWith.resize(objects.size());
	}
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	size_t size = objects.size();

	//If Not Floor
	if (!poses)
	{
		for (int i = 0; i < size; ++i)
		{
			SphereCollider* sphere = objects[i]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
					float3 n = HexagonToSphere(*GetWorldHex(), sphere->GetWorldSphere(), vel);
					if (!n.isEquil(zeroF))
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
			CapsuleCollider* cap = objects[i]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					float3 vel = objects[i]->GetTransform()->GetVelocity();
					Capsule c = cap->GetWorldCapsule();
					float3 n = HexagonToCapsule(*GetWorldHex(), cap->GetWorldCapsule(), vel);
					if (!n.isEquil(zeroF))
					{
						objects[i]->GetTransform()->SetVelocity(vel);
						objects[i]->GetTransform()->SetPosition(c.m_Segment.m_Start);
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
			bool collided = false;
			SphereCollider* sphere = objects[f]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					Sphere s = sphere->GetWorldSphere();
					float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();

					//Top
					if (CheckSphereAABB((int)floor(row * 0.5f) * col, row * col - 1, s))
					{
						//TopTop
						if (CheckSphereAABB((int)floor(row * 0.75f) * col, row * col - 1, s))
						{
							AABB test;
							for (int i = (int)(row * 0.75f); i < row; ++i)
							{
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel);
										if (!n.isEquil(zeroF))
										{
											Physics* op = objects[f]->GetComponent<Physics>();
											if (op)
											{
												op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true,n);
												if (!CollidingWith[f])
												{
													tg->OnCollisionEnter(sphere);
													sphere->GetGameObject()->OnCollisionEnter(this);
													CollidingWith[i] = true;
												}
												collided = true;
											}
										}
										if (collided) break;
									}
								}
								if (collided) break;
							}
						}
						//TopBottom
						else if (CheckSphereAABB((int)floor(row * 0.5f) * col, (int)ceil(row * 0.75f) * col + col - 1, s))
						{
							AABB test;
							for (int i = (int)(row * 0.5f); i < (int)(row * 0.75f); ++i)
							{
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel);
										if (!n.isEquil(zeroF))
										{
											Physics* op = objects[f]->GetComponent<Physics>();
											if (op)
											{
												op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true,n);
												if (!CollidingWith[f])
												{
													tg->OnCollisionEnter(sphere);
													sphere->GetGameObject()->OnCollisionEnter(this);
												}
												collided = true;
											}
											else
											{
												sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
												sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
											}
										}
										if (collided) break;
									}
								}
								if (collided) break;
							}
						}
					}
					//Bottom
					else if (CheckSphereAABB(0, (int)ceil(row * 0.5f) * col + col - 1, s))
					{
						//BottomTop
						if (CheckSphereAABB((int)floor(row * 0.25f) * col, (int)ceil(row * 0.5f) * col + col - 1, s))
						{
							AABB test;
							for (int i = (int)(row * 0.25f); i < (int)(row * 0.5f); ++i)
							{
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel);
										if (!n.isEquil(zeroF))
										{
											Physics* op = objects[f]->GetComponent<Physics>();
											if (op)
											{
												op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true,n);
												if (!CollidingWith[f])
												{
													tg->OnCollisionEnter(sphere);
													sphere->GetGameObject()->OnCollisionEnter(this);
												}
												collided = true;
											}
											else
											{
												sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
												sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
											}
										}
										if (collided) break;
									}
								}
								if (collided) break;
							}
						}
						//BottomBottom
						if (CheckSphereAABB(0, (int)ceil(row * 0.25f) * col + col - 1, s))
						{
							AABB test;
							for (int i = 0; i < (int)(row * 0.25f); ++i)
							{
								test.min = poses[i * col] - float3(1, 10, 1);
								test.max = poses[i * col + col - 1] + float3(1, 20, 1);
								if (SphereToAABB(s, test))
								{
									for (int j = 0; j < col; ++j)
									{
										float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel);
										if (!n.isEquil(zeroF))
										{
											Physics* op = objects[f]->GetComponent<Physics>();
											if (op)
											{
												op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true,n);
												if (!CollidingWith[f])
												{
													tg->OnCollisionEnter(sphere);
													sphere->GetGameObject()->OnCollisionEnter(this);
												}
												collided = true;
											}
											else
											{
												sphere->GetGameObject()->GetTransform()->SetVelocity(vel);
												sphere->GetGameObject()->GetTransform()->SetPosition(s.m_Center);
											}
										}
										if (collided) break;
									}
								}
								if (collided) break;
							}
						}
					}
					if (!collided)
					{
						if (CollidingWith[f])
						{
							CollidingWith[f] = false;
						}
					}
				}
			}

			CapsuleCollider* cap = objects[f]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					Capsule c = cap->GetWorldCapsule();
					float3 vel = objects[f]->GetTransform()->GetVelocity();

					//Top
					if (CheckCapsuleAABB((int)(floor(row * 0.5f)) * col, row * col - 1, c))
					{
						//TopTop
						if (CheckCapsuleAABB((int)(floor(row * 0.75f)) * col, row * col - 1, c))
						{
							//TopTopTop
							if (CheckCapsuleAABB((int)floor(row * 0.83f) * col, row * col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.83f); i < row; ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test,c))
									{
										for (int j = 0; j < col; ++j)
										{
											float3 n = HexagonToCapsule(*GetWorldHex(i * col + j), c, vel);
											if (!n.isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
							//TopTopBottom
							else if (CheckCapsuleAABB((int)floor(row * 0.75f) * col, (int)ceil(row * 0.83f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.75f); i < (int)ceil(row * 0.83f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
						}
						//TopBottom
						else if (CheckCapsuleAABB((int)(floor(row * 0.5f)) * col, (int)(ceil(row * 0.75f)) * col + col - 1, c))
						{
							//TopBottomTop
							if (CheckCapsuleAABB((int)floor(row * 0.63f) * col, (int)ceil(row * 0.75f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.63f); i < (int)ceil(row * 0.75f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
							//TopBottomBottom
							else if (CheckCapsuleAABB((int)floor(row * 0.5f) * col, (int)ceil(row * 0.63f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.5f); i < (int)ceil(row * 0.63f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
						}
					}
					//Bottom
					else if (CheckCapsuleAABB(0, (int)(ceil(row * 0.5f)) * col + col - 1, c))
					{
						//BottomTop
						if (CheckCapsuleAABB((int)(floor(row * 0.25f)) * col, (int)(ceil(row * 0.5f)) * col + col - 1, c))
						{
							//BottomTopTop
							if (CheckCapsuleAABB((int)floor(row * 0.38f) * col, (int)ceil(row * 0.5f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.38f); i < (int)ceil(row * 0.5f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
							//BottomTopBottom
							if (CheckCapsuleAABB((int)floor(row * 0.25f) * col, (int)ceil(row * 0.38f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.25f); i < (int)ceil(row * 0.38f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
						}
						//BottomBottom
						else if (CheckCapsuleAABB(0, (int)(ceil(row * 0.25f)) * col + col - 1, c))
						{
							//BottomBottomTop
							if (CheckCapsuleAABB((int)floor(row * 0.13f) * col, (int)ceil(row * 0.25f) * col + col - 1, c))
							{
								AABB test;
								for (int i = (int)floor(row * 0.13f); i < (int)ceil(row * 0.25f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}
												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
							//BottomBottomBottom
							if (CheckCapsuleAABB(0, (int)ceil(row * 0.13f) * col + col - 1, c))
							{
								AABB test;
								for (int i = 0; i < (int)ceil(row * 0.13f); ++i)
								{
									test.min = poses[i * col] - float3(1, 10, 1);
									test.max = poses[i * col + col - 1] + float3(1, 20, 1);
									if (AABBToCapsule(test, c))
									{
										for (int j = 0; j < col; ++j)
										{
											if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, vel).isEquil(zeroF))
											{
												Physics* op = objects[f]->GetComponent<Physics>();
												if (op)
												{
													op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start, false);
													if (!CollidingWith[f])
													{
														tg->OnCollisionEnter(cap);
														objects[f]->OnCollisionEnter(this);
													}
													collided = true;
												}
												else
												{
													objects[f]->GetTransform()->SetVelocity(vel);
													objects[f]->GetTransform()->SetPosition(c.m_Segment.m_Start);
												}

												otherCapsule = cap;
											}
											else if (otherCapsule == cap)
											{
												otherCapsule->OnCollisionExit(this);
												tg->OnCollisionExit(otherCapsule);
												otherCapsule = nullptr;
											}
										}
									}
								}
							}
						}
					}
					if (!collided)
					{
						if (CollidingWith[f])
						{
							CollidingWith[f] = false;
						}
					}
				}
			}
		}
	}
}