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

void GameObject::InitTransform(DirectX::XMFLOAT4X4 localMatrix, float3 position, float3 rotation, float3 scale, Transform* parent, Transform* child, Transform* sibling)
{
	//set transform
	transform->SetGameObject(this);
	transform->Init(localMatrix, position, rotation, scale, parent, child, sibling);
}

void GameObject::Update(float deltaTime)
{
	for (int i = 0; i < components.size(); ++i)
	{
		if(components[i]->isEnabled())
			components[i]->Update(deltaTime);
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

void GameObject::AddBoxCollider(BoxCollider* box)
{
	components.push_back((Component*)box);
	boxcolliders.push_back(box);
}

void GameObject::AddSphereCollider(SphereCollider* s)
{
	components.push_back((Component*)s);
	spherecolliders.push_back(s);
}


void GameObject::AddCapsuleCollider(CapsuleCollider* c)
{
	components.push_back((Component*)c);
	capsulecolliders.push_back(c);
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

void GameObject::OnCollisionEnter(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnCollisionEnter(e);
	}
}
void GameObject::OnCollisionStay(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnCollisionStay(e);
	}
}
void GameObject::OnCollisionExit(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnCollisionExit(e);
	}
}

void GameObject::OnTriggerEnter(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnTriggerEnter(e);
	}
}
void GameObject::OnTriggerStay(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnTriggerStay(e);
	}
}
void GameObject::OnTriggerExit(Collider* e)
{
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnTriggerExit(e);
	}
}

