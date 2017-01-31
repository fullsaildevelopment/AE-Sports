#pragma once
#include <vector>
#include <string>
#include "AnimationIncludes.h"
#include "KeyFrame.h"

class Animation
{
private:
	AnimType animType;
	std::vector<KeyFrame> keyFrames;
	float totalTime;
	std::string animationName;

public:
	void Init(AnimType type, float time, std::vector<KeyFrame> frames);

	//getters
	KeyFrame* GetFrame(unsigned int index) { return &keyFrames[index]; }
	unsigned int GetNumKeyFrames() { return (unsigned int)keyFrames.size(); }
	AnimType GetType() { return animType; }
	float GetTotalTime() { return totalTime; }
	unsigned int GetNumBones() { return (unsigned int)keyFrames[0].GetBones().size(); }
	std::string GetAnimationName();

	//setters
	void SetAnimationName(std::string animName);
};