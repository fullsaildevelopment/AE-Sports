#include "Countdown.h"
#include "GameObject.h"
#include "UIRenderer.h"
#include "Button.h"
#include "Scene.h"

Countdown::Countdown(Scene* scene)
{
	ResetTimer();
	
	//create object and components
	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	object = new GameObject();
	scene->AddUIObject(object);
	object->Init("Countdown");

	//initialize numbers
	canPlay = false;
	//playSound = true;
}

//basic//
void Countdown::Update(float dt)
{
	timer -= dt;

	if (timer > 2.0f)
	{
		DoAnimation(3);
	}
	else if (timer > 1.0f)
	{
		DoAnimation(2);
	}
	else if (timer > 0.0f)
	{
		DoAnimation(1);
	}
	else
	{
		DoAnimation(0);
		canPlay = true;
	}
}

//misc//
void Countdown::ResetTimer()
{
	timer = timeTilPlay;
}

void Countdown::DoAnimation(int number)
{
	//display the number and scale it while also manipulating alpha
	float ratio = fmodf(timer, 1.0f);

	//play beep sound if first time called for this number
}

//getters//
bool Countdown::CanPlay()
{
	return canPlay;
}

//setters//
void Countdown::SetPlay(bool play)
{
	canPlay = play;
}