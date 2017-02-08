#pragma once
#include <vector>
#include <string>

class Animation;
class Transition;
class AnimatorController;

class State
{
private:
	std::string name;
	AnimatorController* animController; //need a reference to it so we can call its functions
	Animation* animation;
	std::vector<Transition*> transitions;
	bool doLoop;
	float speed;

public:
	//basic//
	void Init(AnimatorController* controller, Animation* anim, bool doLoop, float animSpeed, std::string stateName);
	Transition* Update(float dt);

	//misc//
	void AddTransition(Transition* transition);

	//getters//
	Animation* GetAnimation();
	AnimatorController* GetAnimationController();
	bool DoesItLoop();
	float GetSpeed();
	std::string GetName();

	//setters//
	void SetAnimation(Animation* anim);
};