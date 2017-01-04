#pragma once
#include <vector>
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
	HashString* animationsTable;
public:
	void AddAnimation(Animation animation) { animations.push_back(animation); }

	//setters
	void SetSkeleton(Skeleton skele) { skeleton = skele; }

	//getters
	Animation* GetAnimation(unsigned int index) { return &animations[index]; }
	const BlendInfo* GetBlendInfo(unsigned int animationFrom, unsigned int animationTo) { return &blendInfos[animationFrom][animationTo]; } // TODO: good chance this is wrong
	const Animation* GetDefaultAnimation() { return &animations[defaultAnimation]; }
	Skeleton GetSkeleton() { return skeleton; }
};