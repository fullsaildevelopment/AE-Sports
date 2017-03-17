#include "Countdown.h"
#include "GameObject.h"
#include "UIRenderer.h"
#include "Button.h"
#include "Scene.h"
#include "DeviceResources.h"

Countdown::Countdown(Scene* scene, DeviceResources* devResources)
{
	ResetTimer();
	
	//create object and components
	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	object = new GameObject();
	scene->AddUIObject(object);
	object->Init("CountdownUI");
	button = new Button(true, true, L"3", (unsigned int)strlen("0"), 100.0f, 50.0f, devResources, 0);
	button->setSceneIndex(sceneID);
	button->SetGameObject(object);
	button->showFPS(false);
	button->setPositionMultipliers(0.50f, 0.50f);
	object->AddComponent(button);
	uiRenderer = new UIRenderer();
	uiRenderer->Init(true, 400, devResources, button, L"Consolas", D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
	object->AddComponent(uiRenderer);
	uiRenderer->MakeRTSize();
	button->MakeRect();
	button->setOrigin();
	//button->MakeHandler();
	uiRenderer->InitMetrics();
	button->SetActive(true);
	button->setHelper(scene->GetNumUIObjects());

	//initialize numbers
	canPlay = false;
	//playSound = true;
	curSecond = -1;
}

//basic//
void Countdown::Update(float dt)
{
	timer -= dt;

	if (timer <= 3.0f && curSecond != 3)
	{
		curSecond = 3;
		playSound = true;
	}
	else if (timer <= 2.0f && curSecond != 2)
	{
		curSecond = 2;
		playSound = true;
	}
	else if (timer <= 1.0f && curSecond != 1)
	{
		curSecond = 1;
		playSound = true;
	}
	else if (timer <= 0.0f && curSecond != 0)
	{
		curSecond = 0;
		canPlay = true;
		button->SetEnabled(false);
		uiRenderer->SetEnabled(false);
	}

	DoAnimation(curSecond);
}

//misc//
void Countdown::ResetTimer()
{
	timer = (float)timeTilPlay;
}

void Countdown::DoAnimation(int number)
{
	if (!canPlay)
	{
		//display the number and scale it while also manipulating alpha
		float ratio = fmodf(timer, 1.0f); //will start out big then get smaller

		uiRenderer->SetFontSize(500.0f * ratio);
		uiRenderer->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f * ratio));
		button->setText(to_wstring(number));
		button->MakeRect();
		button->setOrigin();

		//play beep sound if first time called for this number
		if (playSound)
		{
			//switch on number to choose right sound

			playSound = false;
		}
	}
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