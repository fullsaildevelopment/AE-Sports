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

//basic//
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
	transform->Update(deltaTime);
}

//misc//

//automatically sets component's game object pointer
void GameObject::AddComponent(Component* component)
{
	component->SetGameObject(this);
	components.push_back(component);
}

GameObject* GameObject::FindGameObject(std::string name)
{
	GameObject* result = nullptr;
	std::vector<GameObject*>* gameObjects = GetGameObjects();

	for (int i = 0; i < gameObjects->size(); ++i)
	{
		if ((*gameObjects)[i]->GetName() == name)
		{
			result = (*gameObjects)[i];
			break;
		}
	}

	return result;
}

//setters//
void GameObject::SetScene(Scene* tempScene)
{
	scene = tempScene;
}

//getters//
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

std::string GameObject::GetName()
{
	return name;
}

