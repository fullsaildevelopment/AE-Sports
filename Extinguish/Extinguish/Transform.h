#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "Component.h"
#include "vec3.h"
#include "vec4.h"

class Transform : public Component
{
private:
	//std::string name;
	DirectX::XMFLOAT4X4 world;
	std::vector<float3> defaultP;
	float3 position;
	float3 rotation;
	float3 scale;
	float3 velocity;
	DirectX::XMFLOAT4X4 local;
	Transform* parent;
	std::vector<Transform*> children;
	bool bDirty; //if you change the world of the parent, make all of its children dirty. This means their world needs to be updated. This is beneficial because you could make your children dirty multiple times before you actually update world, thus, saving processing

	//move and look members
	float3 originalPosition, originalLookPosition, originalLookRotation, originalRotation, originalForward;
	float moveCurTime, lookCurTime, rotateCurTime;
	float3 moveDestination, lookPosition, rotateRotation;
	float moveTotalTime, lookTotalTime, rotateTotalTime;

	//private helper functions
	void Move();
	void Look();
	void Rotate();

public:
	Transform();
	Transform(GameObject* o);
	~Transform();

	//basic
	void Init(DirectX::XMFLOAT4X4 localMatrix, float3 pos, float3 rot, float3 tempScale, Transform* parent, Transform* child, Transform* sibling);
	void FixedUpdate(float _dt);

	//misc
	void Translate(DirectX::XMFLOAT3 vector);
	void RotateX(float degrees);
	void RotateY(float degrees);
	void RotateZ(float degrees);
	void AddChild(Transform* tempChild);
	void RemoveChild(Transform* abortee);
	void RemoveChildren();
	void AddSibling(Transform* tempSibling);
	void Reset();
	void LookAt(float3 pos, float totalTime);
	void RotateTo(float3 rotation, float totalTime);
	void MoveTo(float3 pos, float totalTime);

	//setters
	void BDirty();
	void SetScale(float3 vector);
	void SetPosition(float3 vector);
	void SetRotation(float3 vector);
	void SetWorld(DirectX::XMFLOAT4X4 matrix);
	void SetLocal(DirectX::XMFLOAT4X4 matrix);
	void SetParent(Transform* pParent);
	void SetVelocity(float3 v);
	void AddVelocity(float3 a);

	//getters
	DirectX::XMFLOAT4X4* GetWorldP();
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT4X4 GetLocal();
	bool GetBDirty();
	float3 GetVelocity();
	float3 GetPosition();
	float3 GetWorldPosition();
	float3 GetRotation();
	float3 GetScale();
	Transform* GetParent();
	Transform* GetChild(int index);
	//Transform* GetSibling();
	float3 GetForwardf3();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetUp();
	float3 GetRightf3();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetRotationDeg();
};