#include "GameObject.h"

//void GameObject::Init(std::string animSet, unsigned int curAnimationIndex, int nextAnimationIndex, bool timeBased)
//{
//	//set blender's anim state
//	//blender.SetAnimationSet(animSet);
//	//blender.SetCurAnimationIndex(curAnimationIndex);
//	//blender.SetNextAnimationIndex(nextAnimationIndex);
//	//blender.Init(timeBased, curAnimationIndex, nextAnimationIndex); //this sets the blenders interpolator
//}

void GameObject::Update(float deltaTime)
{
	//update blender
	//blender.Update(deltaTime, curFrame); //just for now I will set its time to 0.0f

	//send animatedrendernode inverse bind matrices
	//renderNode->SetInverseBindPoses(blender.GetBoneOffsets());
	//renderNode->SetBonesWorlds(blender.GetBonesWorlds());
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}