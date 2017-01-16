#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "Component.h"

class Transform : public Component
{
private:
	//std::string name;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT4X4 local;
	Transform* parent;
	std::vector<Transform*> children;
	bool bDirty; //if you	 change the world of the parent, make all of its children dirty. This means their world needs to be updated. This is beneficial because you could make your children dirty multiple times before you actually update world, thus, saving processing

	//private helper functions
	//void AddSibling(TransformNode2* tempSibling);

	//void AddChild(TransformNode2* tempChild);
public:
	Transform();
	Transform(GameObject* o);
	~Transform();

	//basic
	void Init(DirectX::XMFLOAT4X4 localMatrix, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 tempScale, Transform* parent, Transform* child, Transform* sibling);

	//misc
	void Translate(DirectX::XMFLOAT3 vector);
	void RotateX(float degrees);
	void RotateY(float degrees);
	void RotateZ(float degrees);
	void AddChild(Transform* tempChild);
	void AddSibling(Transform* tempSibling);
	void Update(float dt);

	//setters
	void BDirty();
	void SetScale(DirectX::XMFLOAT3 vector);
	void SetPosition(DirectX::XMFLOAT3 vector);
	void SetRotation(DirectX::XMFLOAT3 vector);
	void SetWorld(DirectX::XMFLOAT4X4 matrix);
	void SetLocal(DirectX::XMFLOAT4X4 matrix);
	void SetParent(Transform* pParent);
	void SetVelocity(DirectX::XMFLOAT3 v);
	void AddVelocity(DirectX::XMFLOAT3 a);

	//getters
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT4X4 GetLocal();
	bool GetBDirty();
	DirectX::XMFLOAT3 GetVelocity();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	Transform* GetParent();
	Transform* GetChild(int index);
	Transform* GetSibling();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
};