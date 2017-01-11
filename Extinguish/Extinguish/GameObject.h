#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Scene.h"
#include "Blender.h"
#include "Renderer.h"
#include "Transform.h"

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
	//basic
	GameObject();
	GameObject(Scene* s);
	~GameObject();

	void Init(std::string name, DirectX::XMFLOAT4X4 localMatrix, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
	void Update(float deltaTime);
	string GetTag() { return tag; };
	//misc
	void AddComponent(Component* component);

	void(*OnCollisionEnter) (Collider*);
	void(*OnCollisionStay) (Collider*);
	void(*OnCollisionExit) (Collider*);

	void(*OnTriggerEnter) (Collider*);
	void(*OnTriggerStay) (Collider*);
	void(*OnTriggerExit) (Collider*);

	template <class T>
	T* GetComponent();
	//setters
	void SetTag(string t) { tag = t; };

	//getters
	Component* GetComponent(unsigned int index) { return components[index]; }
	Transform* GetTransform();

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