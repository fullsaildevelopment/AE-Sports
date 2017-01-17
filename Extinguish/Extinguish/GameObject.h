#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Scene.h"
#include "Blender.h"
#include "Renderer.h"
#include "Transform.h"
#include "InputManager.h"

class Component;
class Collider;
class Scene;

class GameObject
{
private:
	std::string name;
	Transform* transform;
	
	//I could have a vector of components... 
	std::vector<Component*> components;
	string tag;
	//or I could have a pointer to each component. Not as polymorphic necessarily, and it doesn't allow me to have duplicates
	//but maybe only one of each component will be fine for a game of our small scale
	//Renderer* renderer;
	Scene* scene;
public:
	GameObject();
	GameObject(Scene* s);
	~GameObject();

	//basic
	void Init(std::string name);
	void InitTransform(DirectX::XMFLOAT4X4 localMatrix, float3 position, float3 rotation, float3 scale, Transform* parent, Transform* child, Transform* sibling);
	void Update(float deltaTime, InputManager* input);

	//misc
	void AddComponent(Component* component);
	GameObject* FindGameObject(std::string name);

	void(*OnCollisionEnter) (Collider*);
	void(*OnCollisionStay) (Collider*);
	void(*OnCollisionExit) (Collider*);

	void(*OnTriggerEnter) (Collider*);
	void(*OnTriggerStay) (Collider*);
	void(*OnTriggerExit) (Collider*);


	//setters
	void SetTag(string t) { tag = t; };
	void SetScene(Scene* tempScene);

	//getters
	template <class T>
	T* GetComponent();
	template <class T>
	vector<T*> GetComponents();
	Component* GetComponent(unsigned int index) { return components[index]; }
	Transform* GetTransform();
	std::string GetName();
	string GetTag() { return tag; };

	//I want to have the renderer already initialized before I set, so I can keep gameobject simple
	//void SetRenderer(Renderer* node) { renderer = node; }
	vector<GameObject*>* const GetGameObjects();
	GameObject* const GetGameObjects(int i);
	int GetNumObjects();
};

template <class T>
T*	GameObject::GetComponent()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		T* Comp = dynamic_cast<T*>(components[i]);
		if (Comp != nullptr) return Comp;
	}
	return nullptr;
}

template <class T>
vector<T*> GameObject::GetComponents()
{
	vector<T*> v;
	for (size_t i = 0; i < components.size(); ++i)
	{
		T* Comp = dynamic_cast<T*>(components[i]);
		if (Comp != nullptr)
		{
			v.push_back(Comp);
		}
	}
	return v;
}