#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Blender.h"
#include "Renderer.h"

class GameObject
{
private:
	Blender blender;
	//AnimatedRenderNode* renderNode;
	//unsigned int curFrame;
	
	//I could have a vector of components... 
	//std::vector<Component*> components;

	//or I could have a pointer to each component. Not as polymorphic necessarily, and it doesn't allow me to have duplicates
	//but maybe only one of each component will be fine for a game of our small scale
	Renderer* renderer;

public:
	//basic
	//void Init(std::string animSet, unsigned int curAnimationIndex, int nextAnimationIndex, bool timeBased);
	void Update(float deltaTime);

	//misc
	//void AddComponent(Component* component);

	//setters

	//I want to have the renderer already initialized before I set, so I can keep gameobject simple
	void SetRenderer(Renderer* node) { renderer = node; }

	//void SetCurFrame(unsigned int num) { curFrame = num; }
	//void SetBlendInfo(BlendInfo info) { blender.SetBlendInfo(info); }
	//void SetCurAnimation(unsigned int curAnimationIndex) { blender.SetCurAnimationIndex(curAnimationIndex); }
	//void SetNextAnimation(unsigned int nextAnimationIndex) { blender.SetNextAnimationIndex(nextAnimationIndex); }
	//void CreateCurAnimation(bool timeBased) { blender.MakeCurAnimation(timeBased); }
	//void CreateNextAnimation(bool timeBased) { blender.MakeNextAnimation(timeBased); }
};