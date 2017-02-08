#pragma once
#include <vector>
#include <DirectXMath.h>
#include "AnimationSet.h"
#include "AnimationIncludes.h"
#include "Interpolator.h"
#include "HashString.h"
#include "../../Extinguish/ResourceManager.h"
#include <DirectXMath.h>

class Blender
{
private:
	AnimationSet* animationSet;
	BlendInfo blendInfo;
	std::vector<DirectX::XMFLOAT4X4> boneOffsets;
	std::vector<DirectX::XMFLOAT4X4> bonesWorlds;
	Interpolator* curInterpolator;
	Interpolator* nextInterpolator;
	HashString* hashString;
	ResourceManager* resourceManager;
	KeyFrame blendedKeyFrame;
	float curBlendTime;

	//private helper functions
	void Interpolate(KeyFrame* previous, KeyFrame* next, float ratio);

public:
	Blender();
	~Blender();

	void Init(std::string curAnimName, std::string nextAnimName);
	void Update(float time, unsigned int frameIndex);

	//getters
	std::vector<DirectX::XMFLOAT4X4>& GetBoneOffsets() { return boneOffsets; }
	std::vector<DirectX::XMFLOAT4X4> GetBonesWorlds() { return bonesWorlds; }
	Interpolator* GetCurInterpolator();
	Interpolator* GetNextInterpolator();
	AnimationSet* GetAnimationSet();

	//setters
	void SetCurAnimation(std::string animName);
	void SetCurAnimation(int animIndex);
	void SetNextAnimation(std::string animName);
	void SetNextAnimation(int animIndex);
	void SetAnimationSet(AnimationSet* set) { animationSet = set; }
	void SetBlendInfo(BlendInfo info) { blendInfo = info; }
	Skeleton* GetSkeleton() { return animationSet->GetSkeleton(); }
};