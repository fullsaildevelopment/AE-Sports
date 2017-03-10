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

bool HexagonCollider::CheckFloor2Capsule(CapsuleCollider* cap, int f, float _dt)
{
	Capsule c = cap->GetWorldCapsule();
	float3 vel = objects[f]->GetTransform()->GetVelocity();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	bool collided = false;
	//Top
	if (CheckCapsuleAABB((int)(floor(row * 0.49f)) * col, row * col - 1, c))
	{
		//TopTop
		if (CheckCapsuleAABB((int)(floor(row * 0.741f)) * col, row * col - 1, c))
		{
			//TopTopTop
			if (CheckCapsuleAABB((int)floor(row * 0.821f) * col, row * col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row*0.821f), row, cap, c, vel, f,_dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.821f); i < row; ++i)
				{
					test.min = poses[i * col] - float3(1, 10, 1);
					test.max = poses[i * col + col - 1] + float3(1, 20, 1);
					if (AABBToCapsule(test, c))
					{
						for (int j = 0; j < col; ++j)
						{
							float3 n = HexagonToCapsule(*GetWorldHex(i * col + j), c, vel);
							if (!n.isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
			//TopTopBottom
			if (CheckCapsuleAABB((int)floor(row * 0.741f) * col, (int)ceil(row * 0.839f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.741f), (int)ceil(row * 0.839f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.741f); i < (int)ceil(row * 0.83f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
		}
		//TopBottom
		if (CheckCapsuleAABB((int)(floor(row * 0.49f)) * col, (int)(ceil(row * 0.759f)) * col + col - 1, c))
		{
			//TopBottomTop
			if (CheckCapsuleAABB((int)floor(row * 0.621f) * col, (int)ceil(row * 0.759f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.621f), (int)ceil(row * 0.759f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.621f); i < (int)ceil(row * 0.759f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
			//TopBottomBottom
			if (CheckCapsuleAABB((int)floor(row * 0.49f) * col, (int)ceil(row * 0.639f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.49f), (int)ceil(row * 0.639f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.49f); i < (int)ceil(row * 0.639f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
		}
	}
	//Bottom
	if (CheckCapsuleAABB(0, (int)(ceil(row * 0.51f)) * col + col - 1, c))
	{
		//BottomTop
		if (CheckCapsuleAABB((int)(floor(row * 0.241f)) * col, (int)(ceil(row * 0.51f)) * col + col - 1, c))
		{
			//BottomTopTop
			if (CheckCapsuleAABB((int)floor(row * 0.371f) * col, (int)ceil(row * 0.51f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.371f), (int)ceil(row * 0.51f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.371f); i < (int)ceil(row * 0.51f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
			//BottomTopBottom
			if (CheckCapsuleAABB((int)floor(row * 0.241f) * col, (int)ceil(row * 0.389f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.241f), (int)ceil(row * 0.389f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.241f); i < (int)ceil(row * 0.389f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
		}
		//BottomBottom
		if (CheckCapsuleAABB(0, (int)(ceil(row * 0.259f)) * col + col - 1, c))
		{
			//BottomBottomTop
			if (CheckCapsuleAABB((int)floor(row * 0.121f) * col, (int)ceil(row * 0.259f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, (int)floor(row * 0.121f), (int)ceil(row * 0.259f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = (int)floor(row * 0.121f); i < (int)ceil(row * 0.259f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
			//BottomBottomBottom
			if (CheckCapsuleAABB(0, (int)ceil(row * 0.121f) * col + col - 1, c))
			{
				if (CheckCapsule(tg, 0, (int)ceil(row * 0.121f), cap, c, vel, f, _dt))
					collided = true;
				/*AABB test;
				for (int i = 0; i < (int)ceil(row * 0.121f); ++i)
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
									op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
									if (!CollidingWith[f])
									{
										tg->OnCollisionEnter(cap);
										objects[f]->OnCollisionEnter(this);
										CollidingWith[f] = true;
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
				}*/
			}
		}
	}

	if (!collided)
	{
		if (CollidingWith[f])
		{
			tg->OnCollisionExit(cap);
			objects[f]->OnCollisionExit(this);
			CollidingWith[f] = false;
		}
	}
	return collided;
}

bool HexagonCollider::CheckCapsule(GameObject* tg, int _min, int _max, CapsuleCollider* cap, Capsule c, float3 vel, int f, float _dt)
{
	AABB test;
	bool collided = false;
	for (int i = _min; i < _max; ++i)
	{
		test.min = poses[i * col] - float3(3, 10, 3);
		test.max = poses[i * col + col - 1] + float3(3, 20, 3);
		if (AABBToCapsule(test, c))
		{
			for (int j = 0; j < col; ++j)
			{
				float3 n = HexagonToCapsule(*GetWorldHex(i * col + j), c, vel * _dt);
				if (!n.isEquil(zeroF))
				{
					bool apg = false;
					if (!n.isEquil(float3(0, 1, 0)))
						apg = true;
					Physics* op = objects[f]->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(objects[f]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false, n, apg, true);
						if (!CollidingWith[f])
						{
							tg->OnCollisionEnter(cap);
							objects[f]->OnCollisionEnter(this);
							CollidingWith[f] = true;
						}
						collided = true;
					}
				}
			}
		}
	}
	return collided;
}

bool HexagonCollider::CheckFloor2Sphere(SphereCollider* sphere, int f, float _dt)
{
	Sphere s = sphere->GetWorldSphere();
	float3 vel = sphere->GetGameObject()->GetTransform()->GetVelocity();
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();

	bool collided = false;
	//Top
	if (CheckSphereAABB((int)floor(row * 0.49f) * col, row * col - 1, s))
	{
		//TopTop
		if (CheckSphereAABB((int)floor(row * 0.741f) * col, row * col - 1, s))
		{
			if (CheckSphere(tg, (int)floor(row * 0.741f), row, sphere, s, vel, f, _dt))
				collided = true;
			/*AABB test;
			for (int i = (int)floor(row * 0.741f); i < row; ++i)
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
								op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true, n);
								if (!CollidingWith[f])
								{
									tg->OnCollisionEnter(sphere);
									sphere->GetGameObject()->OnCollisionEnter(this);
									CollidingWith[f] = true;
								}
								collided = true;
							}
						}
						if (collided) break;
					}
				}
				if (collided) break;
			}*/
		}
		//TopBottom
		if (CheckSphereAABB((int)floor(row * 0.49f) * col, (int)ceil(row * 0.759f) * col + col - 1, s))
		{
			if (CheckSphere(tg, (int)floor(row * 0.49f), (int)ceil(row * 0.759f), sphere, s, vel, f, _dt))
				collided = true;
			/*AABB test;
			for (int i = (int)floor(row * 0.49f); i < (int)ceil(row * 0.759f); ++i)
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
								op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true, n);
								if (!CollidingWith[f])
								{
									tg->OnCollisionEnter(sphere);
									sphere->GetGameObject()->OnCollisionEnter(this);
									CollidingWith[f] = true;
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
			}*/
		}
	}
	//Bottom
	if (CheckSphereAABB(0, (int)ceil(row * 0.5f) * col + col - 1, s))
	{
		//BottomTop
		if (CheckSphereAABB((int)floor(row * 0.241f) * col, (int)ceil(row * 0.59f) * col + col - 1, s))
		{
			if (CheckSphere(tg, (int)floor(row * 0.241f), (int)ceil(row * 0.59f), sphere, s, vel, f, _dt))
				collided = true;
			/*AABB test;
			for (int i = (int)floor(row * 0.241f); i < (int)ceil(row * 0.59f); ++i)
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
								op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true, n);
								if (!CollidingWith[f])
								{
									tg->OnCollisionEnter(sphere);
									sphere->GetGameObject()->OnCollisionEnter(this);
									CollidingWith[f] = true;
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
			}*/
		}
		//BottomBottom
		if (CheckSphereAABB(0, (int)ceil(row * 0.259f) * col + col - 1, s))
		{
			if (CheckSphere(tg, 0, (int)ceil(row * 0.259f), sphere, s, vel, f, _dt))
				collided = true;
			/*AABB test;
			for (int i = 0; i < (int)ceil(row * 0.259f); ++i)
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
								op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true, n);
								if (!CollidingWith[f])
								{
									tg->OnCollisionEnter(sphere);
									sphere->GetGameObject()->OnCollisionEnter(this);
									CollidingWith[f] = true;
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
			}*/
		}
	}
	if (!collided)
	{
		if (CollidingWith[f])
		{
			CollidingWith[f] = false;
			tg->OnCollisionExit(sphere);
			objects[f]->OnCollisionExit(this);
		}
	}
	return collided;
}

bool HexagonCollider::CheckSphere(GameObject* tg, int _min, int _max, SphereCollider* sphere, Sphere s, float3 vel, int f, float _dt)
{
	bool collided = false;
	AABB test;
	for (int i = _min; i < _max; ++i)
	{
		test.min = poses[i * col] - float3(1, 10, 1);
		test.max = poses[i * col + col - 1] + float3(1, 20, 1);
		if (SphereToAABB(s, test))
		{
			for (int j = 0; j < col; ++j)
			{
				float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), vel * _dt);
				if (!n.isEquil(zeroF))
				{
					Physics* op = objects[f]->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(objects[f]->GetTransform(), vel, s.m_Center, true, n);
						if (!CollidingWith[f])
						{
							tg->OnCollisionEnter(sphere);
							sphere->GetGameObject()->OnCollisionEnter(this);
							CollidingWith[f] = true;
						}
						collided = true;
					}
				}
			}
		}
	}
	return collided;
}

void HexagonCollider::FixedUpdate(float _dt)
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
					Sphere s = sphere->GetWorldSphere();
					float3 n = HexagonToSphere(*GetWorldHex(), s, vel * _dt);
					if (!n.isEquil(zeroF))
					{
						Physics* op = objects[i]->GetComponent<Physics>();
						if (op)
						{
							op->HandlePhysics(objects[i]->GetTransform(), vel, s.m_Center, true, n);
							if (!CollidingWith[i])
							{
								tg->OnCollisionEnter(sphere);
								sphere->GetGameObject()->OnCollisionEnter(this);
								CollidingWith[i] = true;
							}
							continue;
						}
					}
					if (CollidingWith[i])
					{
						tg->OnCollisionExit(sphere);
						sphere->GetGameObject()->OnCollisionExit(this);
						CollidingWith[i] = false;
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
					float3 n = HexagonToCapsule(*GetWorldHex(), cap->GetWorldCapsule(), vel * _dt);
					if (!n.isEquil(zeroF))
					{
						bool apg = false;
						if (!n.isEquil(float3(0, 1, 0)))
							apg = true;
						Physics* op = objects[i]->GetComponent<Physics>();
						if (op)
						{
							op->HandlePhysics(objects[i]->GetTransform(), vel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false, n, apg, true);
							if (!CollidingWith[i])
							{
								tg->OnCollisionEnter(cap);
								objects[i]->OnCollisionEnter(this);
								CollidingWith[i] = true;
							}
							continue;
						}
					}
					if (CollidingWith[i])
					{
						tg->OnCollisionExit(cap);
						cap->GetGameObject()->OnCollisionExit(this);
						CollidingWith[i] = false;
					}
				}
			}
		}
	}
	//If Floor
	else
	{
		for (int f = 0; f < size; ++f)
		{
			bool collided = false;
			SphereCollider* sphere = objects[f]->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					CheckFloor2Sphere(sphere, f, _dt);
				}
			}

			CapsuleCollider* cap = objects[f]->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					CheckFloor2Capsule(cap, f, _dt);
				}
			}
		}
	}
}