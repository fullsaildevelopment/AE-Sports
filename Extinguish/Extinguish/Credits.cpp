#include "Credits.h"
#include "Button.h"
#include "UIRenderer.h"
#include "InputDownEvent.h"
#include "LoadSceneEvent.h"


Credits::Credits()
{
}


Credits::~Credits()
{
	for (unsigned int i = 0; i < theText.size(); ++i)
	{
		delete theText[i];
		delete textRenderers[i];
	}
}


void Credits::Init(DeviceResources * devResources)
{
	// format still being tweeked
	for (unsigned int i = 0; i < 5; ++i) {
		GameObject * text = new GameObject();
		text->Init("TheCredits");
		Button * theButton = new Button(true, true, L"", (unsigned int)strlen(""), 500.0f, 100.0f, devResources, 0);
		theButton->SetGameObject(text);
		theButton->setTimer(true);
		theButton->showFPS(false);
		theButton->setPositionMultipliers(0.5f, 0.15f + (0.15f * (float)i));
		text->AddComponent(theButton);

		UIRenderer * tRender = new UIRenderer();
		tRender->Init(true, 35.0f, devResources, theButton, L"Consolas", D2D1::ColorF(0.8f, 0.8f, 0.8f, 0.0f));
		text->AddComponent(tRender);
		tRender->MakeRTSize();
		theButton->MakeRect();
		tRender->InitMetrics();
		theButton->setOrigin();
		tRender->setOpacity(0.0f);

		theObjs.push_back(text);
		theText.push_back(theButton);
		textRenderers.push_back(tRender);
	}
	// labels
	for (unsigned int i = 0; i < 5; ++i) {
		GameObject * text = new GameObject();
		Button * theButton = new Button(true, true, L"", (unsigned int)strlen(""), 500.0f, 100.0f, devResources, 0);
		theButton->SetGameObject(text);
		theButton->setTimer(true);
		theButton->showFPS(false);
		theButton->setPositionMultipliers(0.5f, 0.10f + (0.15f * (float)i));
		text->AddComponent(theButton);

		UIRenderer * tRender = new UIRenderer();
		tRender->Init(true, 45.0f, devResources, theButton, L"Consolas", D2D1::ColorF(0.8f, 0.8f, 0.8f, 0.0f));
		text->AddComponent(tRender);
		tRender->MakeRTSize();
		theButton->MakeRect();
		tRender->InitMetrics();
		theButton->setOrigin();

		theObjs.push_back(text);
		theText.push_back(theButton);
		textRenderers.push_back(tRender);
	}

	SetNames();
	NextSet();
}

void Credits::Render()
{
	textRenderers[0]->getUIDevCon()->BeginDraw();
	for (unsigned int i = 0; i < textRenderers.size(); ++i)
	{
		textRenderers[i]->Render();
	}
	textRenderers[0]->getUIDevCon()->EndDraw();
}

void Credits::Update(float _dt)
{
	// timer
	if (fadeIn)
	{
		if (nextName == 5)
			textRenderers[0]->setOpacity(textRenderers[0]->getOpacity() + _dt);
		fadeInOut += _dt;
		if (fadeInOut >= 1.0f)
		{
			fadeInOut = 1.0f;
			waitForIt += _dt;
			if (waitForIt >= 4.5f)
			{
				waitForIt = 0.0f;
				fadeIn = false;
			}
		}
		else
		{
			for (unsigned int i = 0; i < 10; ++i)
			{
				textRenderers[i]->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, fadeInOut));
				textRenderers[i]->Update(_dt);
			}
		}
	}
	else
	{
		if (nextName == 5)
			textRenderers[0]->setOpacity(textRenderers[0]->getOpacity() - _dt);
		fadeInOut -= _dt;
		if (fadeInOut <= 0.0f)
		{
			fadeInOut = 0.0f;
			fadeIn = true;
			// adjust text
			NextSet();
		}
		else
		{
			for (unsigned int i = 0; i < 10; ++i)
			{
				textRenderers[i]->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, fadeInOut));
				textRenderers[i]->Update(_dt);
			}
		}
	}
}

void Credits::Shutdown()
{
	
}

void Credits::HandleEvent(Event* e)
{
	// press space, enter, or click to go back to main menu
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		InputManager * input = inputDownEvent->GetInput();

		if (input->GetKeyboardDown())
		{
			ReturnToMenu();
		}
	}
}

void Credits::SetNames()
{
	// PRODUCER
	labels.push_back(L"PRODUCER");
	// Jason Hinders
	names.push_back(L"Jason Hinders");

	// AE SPORTS -- icon
	// Tom Devaney
	names.push_back(L"Tom Devany");
	// Nicholas Gilden
	names.push_back(L"Nicholas Gilden");
	// Linda Juarez
	names.push_back(L"Linda Juarez");
	// Samantha Reed
	names.push_back(L"Samantha Reed");
	// CHARACTER MODEL
	labels.push_back(L"CHARACTER ARTIST");
	// Quest Matthews-Martial
	names.push_back(L"Quest Matthews-Martial");
	// ANIMATIONS
	labels.push_back(L"RIGGING & ANIMATIONS");
	// Kris Ducote
	names.push_back(L"Kris Ducote");
	// ARENA
	labels.push_back(L"ARENA ARTIST");
	// Ryan Martin
	names.push_back(L"Ryan Martin");
	// POWER UP MODELS
	labels.push_back(L"POWER UP ARTIST");
	// Zack Ferry
	names.push_back(L"Zack Ferry");
	// MUSIC
	labels.push_back(L"MUSIC");
	// Alice Kim
	names.push_back(L"'Alice' Hyeon Jo Kim");
}

void Credits::NextSet()
{
	if (nextName < names.size() - 1)
	{
		if (nextName == 0)
		{
			// only producers
			theText[7]->setText(labels[0]);
			theText[3]->setText(names[nextName]);
			nextName = 1;
		}
		else if (nextName == 1)
		{
			// AE SPORTS
			theText[7]->setText(L"");
			for (nextName; nextName != 5; ++nextName)
			{
				theText[nextName]->setText(names[nextName]);
			}

			theText[0]->setHeight(300.0f);
			theText[0]->setWidth(300.0f);
			theText[0]->MakeRect();
			textRenderers[0]->DecodeBitmap(L"../Assets/UI/Logo.png");

		}
		else
		{
			// assets
			textRenderers[0]->RemoveBitmap();
			theText[0]->setHeight(100.0f);
			theText[0]->setWidth(500.0f);
			theText[0]->MakeRect();
			unsigned int x = 5;
			for (unsigned int i = 0; nextName < names.size(); ++i, ++nextName, ++x)
			{
				theText[x]->setText(labels[i + 1]);
				theText[i]->setText(names[nextName]);
			}
		}
	}
	else
	{
		ReturnToMenu();
	}
}

void Credits::ReturnToMenu()
{
	nextName = 0;
	fadeInOut = 0.0f;
	fadeIn = true;

	ResourceManager::GetSingleton()->SetPaused(false);
	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("Menu");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;

}