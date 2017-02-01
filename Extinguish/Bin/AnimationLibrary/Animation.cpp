#include "Animation.h"

void Animation::Init(AnimType type, float time, std::vector<KeyFrame> frames)
{
	animType = type;
	totalTime = time;
	keyFrames = frames;
}

//getters//
std::string Animation::GetAnimationName()
{
	return animationName;
}

//setters//
void Animation::SetAnimationName(std::string animName)
{
	animationName = animName;
}