#include "HexagonCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "Physics.h"
#include "GameObject.h"

HexagonCollider::HexagonCollider(GameObject* o, float d, float _height, ED2Mesh* colMesh) : Collider(o, false, CTHex)
{
	mesh = colMesh;
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

HexagonCollider::HexagonCollider(int _row, int _col, float3* positions, float _height, float d, GameObject* o, ED2Mesh* colMesh) : Collider(o, false, CTHex)
{
	mesh = colMesh;
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

	AABB checksphereaabb;
	AABB sphcheckaabb;
bool HexagonCollider::CheckSphereAABB(int _min, int _max, Sphere s)
{
	checksphereaabb.max = poses[_max] + float3(1, 20, 1);
	checksphereaabb.min = poses[_min] - float3(1, 10, 1);
	sphcheckaabb.min = s.m_Center - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	sphcheckaabb.max = s.m_Center + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(sphcheckaabb, checksphereaabb);
}

bool HexagonCollider::CheckSphereAABB(AABB test, Sphere s)
{
	AABB cap;
	cap.min = s.m_Center - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	cap.max = s.m_Center + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(test, cap);
}

AABB checkCapaabb;
AABB capcheckaabb;
bool HexagonCollider::CheckCapsuleAABB(int _min, int _max, Capsule s)
{
	checkCapaabb.max = poses[_max] + float3(1, 20, 1);
	checkCapaabb.min = poses[_min] - float3(1, 10, 1);
	capcheckaabb.min = s.m_Segment.m_Start - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	capcheckaabb.max = s.m_Segment.m_End + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(checkCapaabb, capcheckaabb);
}

bool HexagonCollider::CheckCapsuleAABB(AABB test, Capsule s)
{
	AABB cap;
	cap.min = s.m_Segment.m_Start - float3(s.m_Radius, s.m_Radius, s.m_Radius);
	cap.max = s.m_Segment.m_End + float3(s.m_Radius, s.m_Radius, s.m_Radius);
	return AABBtoAABB(test, cap);
}

bool HexagonCollider::CheckFloor2Capsule(CapsuleCollider* cap, int f, float dt)
{
	GameObject* ob = objects[f];
	Capsule c = cap->GetWorldCapsule();
	float3 pastPos = ob->GetComponent<Physics>()->pastPos;
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	float smallestT = 100;
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
				if (CheckCapsule(tg,ob, (int)floor(row*0.821f), row, cap, c, pastPos, f,dt, smallestT))
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
							float3 n = HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos);
							if (!n.isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg,ob, (int)floor(row * 0.741f), (int)ceil(row * 0.839f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg,ob, (int)floor(row * 0.621f), (int)ceil(row * 0.759f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg, ob,(int)floor(row * 0.49f), (int)ceil(row * 0.639f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg, ob,(int)floor(row * 0.371f), (int)ceil(row * 0.51f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg, ob,(int)floor(row * 0.241f), (int)ceil(row * 0.389f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg, ob,(int)floor(row * 0.121f), (int)ceil(row * 0.259f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
				if (CheckCapsule(tg, ob, 0, (int)ceil(row * 0.121f), cap, c, pastPos, f, dt, smallestT))
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
							if (!HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos).isEquil(zeroF))
							{
								Physics* op = objects[f]->GetComponent<Physics>();
								if (op)
								{
									op->HandlePhysics(objects[f]->GetTransform(), pastPos, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false);
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
									objects[f]->GetTransform()->SetVelocity(pastPos);
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
			ob->OnCollisionExit(this);
			CollidingWith[f] = false;
		}
	}
	return collided;
}

bool HexagonCollider::CheckCapsule(GameObject* tg, GameObject* ob, int _min, int _max, CapsuleCollider* cap, Capsule c, float3 pastPos, int f, float dt, float& St)
{
	AABB test;
	bool collided = false;
	float smallT = St;
	for (int i = _min; i < _max; ++i)
	{
		test.min = poses[i * col] - float3(2, 2, 2);
		test.max = poses[i * col + col - 1] + float3(2, 22, 2);
		if (AABBToCapsule(test, c))
		{
			for (int j = 0; j < col; ++j)
			{
				float3 n = HexagonToCapsule(*GetWorldHex(i * col + j), c, pastPos + cap->GetCapsule().m_Segment.m_Start, smallT, mesh);
				if (!n.isEquil(zeroF) && smallT < St)
				{
					bool apg = false;
					if (!n.isEquil(float3(0, 1, 0)))
						apg = true;
					Physics* op = ob->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(ob->GetTransform(), ob->GetTransform()->GetVelocity(), c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false, n, apg, true);
						if (!CollidingWith[f])
						{
							tg->OnCollisionEnter(cap);
							ob->OnCollisionEnter(this);
							CollidingWith[f] = true;
						}
						if (CollidingWith[f])
						{
							tg->OnCollisionStay(cap);
							ob->OnCollisionStay(this);
						}
						collided = true;
					}
				}
			}
		}
	}
	return collided;
}

bool HexagonCollider::CheckFloor2Sphere(SphereCollider* sphere, int f, float dt)
{
	GameObject* ob = objects[f];
	Sphere s = sphere->GetWorldSphere();
	float3 pastPos = objects[f]->GetComponent<Physics>()->pastPos;
	GameObject* tg = GetGameObject();
	Transform* tgt = tg->GetTransform();
	float smallestT = 100;
	bool collided = false;
	//Top
	if (CheckSphereAABB((int)floor(row * 0.49f) * col, row * col - 1, s))
	{
		//TopTop
		if (CheckSphereAABB((int)floor(row * 0.741f) * col, row * col - 1, s))
		{
			if (CheckSphere(tg, ob, (int)floor(row * 0.741f), row, sphere, s, pastPos, f, dt, smallestT))
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
						float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), pastPos);
						if (!n.isEquil(zeroF))
						{
							Physics* op = objects[f]->GetComponent<Physics>();
							if (op)
							{
								op->HandlePhysics(objects[f]->GetTransform(), pastPos, s.m_Center, true, n);
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
			if (CheckSphere(tg, ob, (int)floor(row * 0.49f), (int)ceil(row * 0.759f), sphere, s, pastPos, f, dt, smallestT))
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
						float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), pastPos);
						if (!n.isEquil(zeroF))
						{
							Physics* op = objects[f]->GetComponent<Physics>();
							if (op)
							{
								op->HandlePhysics(objects[f]->GetTransform(), pastPos, s.m_Center, true, n);
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
								sphere->GetGameObject()->GetTransform()->SetVelocity(pastPos);
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
			if (CheckSphere(tg, ob,(int)floor(row * 0.241f), (int)ceil(row * 0.59f), sphere, s, pastPos, f, dt, smallestT))
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
						float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), pastPos);
						if (!n.isEquil(zeroF))
						{
							Physics* op = objects[f]->GetComponent<Physics>();
							if (op)
							{
								op->HandlePhysics(objects[f]->GetTransform(), pastPos, s.m_Center, true, n);
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
								sphere->GetGameObject()->GetTransform()->SetVelocity(pastPos);
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
			if (CheckSphere(tg, ob, 0, (int)ceil(row * 0.259f), sphere, s, pastPos, f, dt, smallestT))
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
						float3 n = HexagonToSphere(*GetWorldHex(i * col + j), sphere->GetWorldSphere(), pastPos);
						if (!n.isEquil(zeroF))
						{
							Physics* op = objects[f]->GetComponent<Physics>();
							if (op)
							{
								op->HandlePhysics(objects[f]->GetTransform(), pastPos, s.m_Center, true, n);
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
								sphere->GetGameObject()->GetTransform()->SetVelocity(pastPos);
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

bool HexagonCollider::CheckSphere(GameObject* tg, GameObject* ob, int _min, int _max, SphereCollider* sphere, Sphere s, float3 pastPos, int f, float dt, float& St)
{
	bool collided = false;
	AABB test;
	float smallT = St;
	for (int i = _min; i < _max; ++i)
	{
		test.min = poses[i * col] - float3(1, 10, 1);
		test.max = poses[i * col + col - 1] + float3(1, 20, 1);
		if (SphereToAABB(s, test))
		{
			for (int j = 0; j < col; ++j)
			{
				float3 n = HexagonToSphere(*GetWorldHex(i * col + j), s, pastPos, smallT);
				if (!n.isEquil(zeroF) && smallT < St)
				{
					Physics* op = ob->GetComponent<Physics>();
					if (op)
					{
						op->HandlePhysics(ob->GetTransform(), ob->GetTransform()->GetVelocity(), s.m_Center, true, n);
						if (!CollidingWith[f])
						{
							tg->OnCollisionEnter(sphere);
							ob->OnCollisionEnter(this);
							CollidingWith[f] = true;
						}
						if (CollidingWith[f])
						{
							tg->OnCollisionStay(sphere);
							ob->OnCollisionStay(this);
						}
						collided = true;
					}
				}
			}
		}
	}
	return collided;
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
	GameObject* ob;

	//If Not Floor
	if (!poses)
	{
		for (int i = 0; i < size; ++i)
		{
			ob = objects[i];
			SphereCollider* sphere = ob->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					float3 pastPos = ob->GetComponent<Physics>()->pastPos;
					Sphere s = sphere->GetWorldSphere();
					float t = 100;
					float3 n = HexagonToSphere(*GetWorldHex(), s, pastPos, t, mesh);
					if (!n.isEquil(zeroF))
					{
						Physics* op = ob->GetComponent<Physics>();
						if (op)
						{
							op->HandlePhysics(ob->GetTransform(), ob->GetTransform()->GetVelocity(), s.m_Center, true, n);
							if (!CollidingWith[i])
							{
								tg->OnCollisionEnter(sphere);
								ob->OnCollisionEnter(this);
								CollidingWith[i] = true;
							}
							continue;
						}
					}
					if (CollidingWith[i])
					{
						tg->OnCollisionExit(sphere);
						ob->OnCollisionExit(this);
						CollidingWith[i] = false;
					}
				}
			}
			CapsuleCollider* cap = ob->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					float3 pastPos = ob->GetComponent<Physics>()->pastPos + cap->GetCapsule().m_Segment.m_Start;
					float3 nvel = ob->GetTransform()->GetVelocity();
					Capsule c = cap->GetWorldCapsule();
					float t = 100;
					float3 n = HexagonToCapsule(*GetWorldHex(), c, pastPos, t, mesh);
					if (!n.isEquil(zeroF))
					{
						bool apg = false;
						if (!n.isEquil(float3(0, 1, 0)))
							apg = true;
						Physics* op = ob->GetComponent<Physics>();
						if (op)
						{
							op->HandlePhysics(ob->GetTransform(), nvel, c.m_Segment.m_Start - cap->GetCapsule().m_Segment.m_Start, false, n, apg, true);
							if (!CollidingWith[i])
							{
								tg->OnCollisionEnter(cap);
								ob->OnCollisionEnter(this);
								CollidingWith[i] = true;
							}
							continue;
						}
					}
					if (CollidingWith[i])
					{
						tg->OnCollisionExit(cap);
						ob->OnCollisionExit(this);
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
			ob = objects[f];
			bool collided = false;
			SphereCollider* sphere = ob->GetComponent<SphereCollider>();
			if (sphere)
			{
				if (!sphere->isTrigger() && sphere->isEnabled())
				{
					CheckFloor2Sphere(sphere, f, dt);
				}
			}

			CapsuleCollider* cap = ob->GetComponent<CapsuleCollider>();
			if (cap)
			{
				if (!cap->isTrigger() && cap->isEnabled())
				{
					CheckFloor2Capsule(cap, f, dt);
				}
			}
		}
	}
}