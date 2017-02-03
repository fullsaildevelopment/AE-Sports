#pragma once
#include <vector>

class Animation;
class Transition;

class State
{
private:
	Animation* animation;
	std::vector<Transition*> transitions;

public:
	void Update();

};