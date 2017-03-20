#pragma once
#include "Component.h"

class GameObject;
class UIRenderer;
class Button;
class Scene;
class DeviceResources;

class Countdown : public Component
{
private:
	//regular members
	float timer;
	bool canPlay;
	int curSecond;
	bool playSound;
	GameObject* object;
	UIRenderer* uiRenderer;
	Button* button;

	//const members
	const int timeTilPlay = 3;
	const int sceneID = 2;

	//private helper functions
	void SendPlayEvent(bool toggle);

public:
	Countdown(Scene* scene, DeviceResources* devResources);

	//basic
	void Update(float dt) override;

	//misc
	void Reset();
	void DoAnimation(int number);

	//getters
	bool CanPlay();

	//setters
	void SetPlay(bool play);
};