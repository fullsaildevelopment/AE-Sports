#pragma once
#include <vector>

class Animation;
class Transition;

class State
{
private:
	Animation* animation;
	std::vector<Transition*> transitions;
	//bool loop;
	//float speed;

public:
	//basic//
	Transition* Update(float dt);

	//getters//
	Animation* GetAnimation();

	//setters//
	void SetAnimation(Animation* anim);
};