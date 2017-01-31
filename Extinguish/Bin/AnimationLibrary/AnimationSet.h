#pragma once
#include <vector>
#include <string>
#include "Animation.h"
#include "Skeleton.h"
#include "AnimationIncludes.h"
#include "HashString.h"

class AnimationSet
{
private:
	std::vector<Animation> animations;
	Skeleton skeleton;
	std::vector<std::vector<BlendInfo>> blendInfos;
	unsigned int defaultAnimation;
	//should make a hashstring for every animation set. When animation is added.
	HashString animationsTable;
public:
	void AddAnimation(Animation animation, std::string animationName);

	//setters
	void SetSkeleton(Skeleton skele);

	//getters
	Animation* GetAnimation(std::string animationName);
	Animation* GetAnimation(int animationIndex);
	const BlendInfo* GetBlendInfo(unsigned int animationFrom, unsigned int animationTo); // TODO: good chance this is wrong
	const Animation* GetDefaultAnimation();
	Skeleton* GetSkeleton();
	int GetAnimationIndex(std::string animationName);
};