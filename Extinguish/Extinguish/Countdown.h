#pragma once
#include "Component.h"

class GameObject;
class UIRenderer;
class Button;
class Scene;

class Countdown : public Component
{
private:
	//regular members
	float timer;
	bool canPlay;
	//bool playSound;
	GameObject* object;
	UIRenderer* uiRenderer;
	Button* button;

	//const members
	const int timeTilPlay = 3;
public:
	Countdown(Scene* scene);

	//basic
	void Update(float dt) override;

	//misc
	void ResetTimer();
	void DoAnimation(int number);

	//getters
	bool CanPlay();

	//setters
	void SetPlay(bool play);
};