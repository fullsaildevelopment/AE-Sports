#include "GameObject.h"

GameObject::GameObject()
{
	transform = new Transform();
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
void GameObject::Init(std::string name)
{
	this->name = name;
}

void GameObject::InitTransform(DirectX::XMFLOAT4X4 localMatrix, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, Transform* parent, Transform* child, Transform* sibling)
{
	//set transform
	transform->Init(localMatrix, position, rotation, scale, parent, child, sibling);
	transform->SetGameObject(this);
}

void GameObject::Update(float deltaTime, InputManager* input)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->Update(deltaTime, input);
	}
}

//automatically sets component's game object pointer
void GameObject::AddComponent(Component* component)
{
	component->SetGameObject(this);
	components.push_back(component);
}

//getters
vector<GameObject*>* const GameObject::GetGameObjects() 
{ 
	return scene->GetGameObjects(); 
}

Transform* GameObject::GetTransform()
{
	return transform;
}

std::string GameObject::GetName()
{
	return name;
}

