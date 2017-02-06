#pragma once
#include <vector>

class Animation;
class Transition;
class AnimatorController;

class State
{
private:
	AnimatorController* animController; //need a reference to it so we can call its functions
	Animation* animation;
	std::vector<Transition*> transitions;
	bool doLoop;
	float speed;

public:
	//basic//
	void Init(AnimatorController* controller, Animation* anim, bool doLoop, float animSpeed);
	Transition* Update(float dt);

	//getters//
	Animation* GetAnimation();
	AnimatorController* GetAnimationController();
	bool DoesItLoop();
	float GetSpeed();

	//setters//
	void SetAnimation(Animation* anim);
};