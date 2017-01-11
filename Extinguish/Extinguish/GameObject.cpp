#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
}

GameObject::GameObject(Scene* s)
{
	transform = new Transform();
	scene = s;
}

GameObject::~GameObject()
{
	delete transform;

	for (int i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}
}

//basic
void GameObject::Init(std::string name, DirectX::XMFLOAT4X4 localMatrix, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	this->name = name;

	//set transform
	transform->Init(localMatrix, position, rotation, scale);
}

void GameObject::Update(float deltaTime)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->Update(deltaTime);
	}
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}

//getters
vector<GameObject*>* const GameObject::GetGameObjects() 
{ 
	return scene->GetGameObjects(); 
}

GameObject* const GameObject::GetGameObjects(int i)
{
	return scene->GetGameObjects(i);
}

int GameObject::GetNumObjects()
{
	return scene->GetNumObjects();
}

Transform* GameObject::GetTransform()
{
	return transform;
}

