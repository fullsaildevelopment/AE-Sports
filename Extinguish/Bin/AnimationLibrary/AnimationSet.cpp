#include "AnimationSet.h"

void AnimationSet::AddAnimation(Animation animation, std::string animationName)
{
	animations.push_back(animation);
	animationsTable.Insert(animationName);
}

//setters
void AnimationSet::SetSkeleton(Skeleton skele)
{
	skeleton = skele;
}

//getters
Animation* AnimationSet::GetAnimation(std::string animationName)
{
	Animation* result = nullptr;

	int index = animationsTable.GetKey(animationName);

	if (index != -1)
	{
		result = &animations[index];
	}

	return result;
}

const BlendInfo* AnimationSet::GetBlendInfo(unsigned int animationFrom, unsigned int animationTo)
{
	return &blendInfos[animationFrom][animationTo];
} 

const Animation* AnimationSet::GetDefaultAnimation()
{
	return &animations[defaultAnimation];
}

Skeleton AnimationSet::GetSkeleton()
{
	return skeleton;
}