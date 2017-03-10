#include "Button.h"
#include "Game.h"
#include "Server Wrapper.h"
#include "Client Wrapper.h"
#include "EventDispatcher.h"
#include "LoadSceneEvent.h"
#include "SoundEvent.h"
#include "SoundEngine.h"

void StartGame()
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
		Game::server.StartGame();

	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("FirstLevel");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;

	SoundEvent* soundEvent = new SoundEvent();
	soundEvent->Init(AK::EVENTS::STOP_BACKBOARD_BOUNCE_SONG, 1);
	EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
	delete soundEvent;
}

void StartLobby()
{
	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("Lobby");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;
}

void StartServer()
{
	if (Game::server.init("127.0.0.1", 60000) == 1)
	{
		ResourceManager::GetSingleton()->SetMultiplayer(true);
		ResourceManager::GetSingleton()->SetServer(true);

		Game::client.init("127.0.0.1", 60001);
		Game::client.setIsServer();

		Game::server.setObjIDs(Game::objIDs[0],
			Game::objIDs[1],
			Game::objIDs[2],
			Game::objIDs[3],
			Game::objIDs[4],
			Game::objIDs[5],
			Game::objIDs[6],
			Game::objIDs[7]
		);

		StartLobby();
	}
}

void JoinServer()
{
	ResourceManager::GetSingleton()->SetMultiplayer(true);
	ResourceManager::GetSingleton()->SetServer(false);
	Game::client.init("127.0.0.1", 60001);

	StartLobby();
}

void ShutdownGame()
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (ResourceManager::GetSingleton()->IsServer())
		{
			Game::server.stop();
			while (Game::server.run() != 0) {
				if (Game::client.run() == 0)
					break;
			}
		}
		else
		{
			Game::client.sendStop();

			while (Game::client.run() != 0) {}
		}
	}
	
	Game::returnResult = 0;
}

void ReturnToMenu()
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (ResourceManager::GetSingleton()->IsServer())
		{
			Game::server.stop();
			while (Game::server.run() != 0) {
				if (Game::client.run() == 0)
					break;
			}

			ResourceManager::GetSingleton()->SetMultiplayer(false);
		}
		else
		{
			Game::client.sendStop();

			while (Game::client.run() != 0) {}
			ResourceManager::GetSingleton()->SetMultiplayer(false);
			ResourceManager::GetSingleton()->SetServer(true);
		}
	}
	LoadSceneEvent* event = new LoadSceneEvent();
	event->Init("Menu");
	EventDispatcher::GetSingleton()->DispatchTo(event, "Game");
	delete event;
}

void ChangeTeamA()
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
		Game::client.changeTeamA();
	else
		Game::team = Game::TEAM_A;
}

void ChangeTeamB()
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
		Game::client.changeTeamB();
	else
		Game::team = Game::TEAM_B;
}

//Button::~Button()
//{
//	if (buttonType == EXIT)
//	{
//		if (ResourceManager::GetSingleton()->IsMultiplayer())
//		{
//			if (ResourceManager::GetSingleton()->IsServer())
//			{
//				Game::server.stop();
//				while (Game::server.run() != 0) {
//					if (Game::client.run() == 0)
//						break;
//				}
//			}
//			else
//			{
//				Game::client.sendStop();
//
//				while (Game::client.run() != 0) {}
//			}
//		}
//	}
//}

Button::Button(bool active, bool clickable, wchar_t * newText, unsigned int length, float _width, float _height,
	DeviceResources * resources, unsigned int type) {
	isActive = active;
	isClickable = clickable;
	text = newText;
	textLength = length;
	height = _height;
	width = _width;
	//eventFunction = func;
	buttonType = (BUTTON_TYPE)type;

	HRESULT hr = resources->GetWriteFactory()->CreateTypography(pTypography.GetAddressOf());
	if (SUCCEEDED(hr))
	{
		DWRITE_FONT_FEATURE fontFeature = { DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7,
			1 };
		hr = pTypography->AddFontFeature(fontFeature);
	}

	setButtonType();
}

void Button::MakeHandler()
{
	GameObject * object = GetGameObject();
	EventDispatcher::GetSingleton()->RegisterHandler(this, object->GetName());
}

Button::Button(bool active, bool clickable, char * newText, unsigned int length) {
	isActive = active;
	isClickable = clickable;
	ctext = newText;
	textLength = length;
}

void Button::Update(float _dt)
{
	if (isTimer)
	{
		time = Game::Time;
		setText(getWTime());
	}

	if (clickCooldown >= 0.0f)
		clickCooldown -= _dt;

	if (showFps)
	{
		int _fps = (int)(1 / _dt);

		fps = to_string(_fps);
		fps += " FPS";
	}
}


void Button::HandleEvent(Event* e)
{
	if (isClickable && isActive && sceneIndex == Game::currentScene)
	{
		InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

		if (inputDownEvent && isActive)
		{
			InputManager * input = inputDownEvent->GetInput();

			int mouseX = input->GetMouseX();
			int mouseY = input->GetMouseY();

			if (mouseX > (int)rect.left && mouseX < (int)rect.right && mouseY > (int)rect.top && mouseY < (int)rect.bottom)
			{
				if (input->GetMouseDown()[0] && clickCooldown <= 0.0f)
				{
					if (buttonType == RETURN || buttonType == HOST || buttonType == JOIN)
					{
						clickCooldown = 2.0f;
					}
					if (eventFunction)
					{
						eventFunction();
						if (buttonType == CHANGE_TEAM_A)
						{
							GameObject * otherteam = GetGameObject()->GetUIGameObjects(helperIndex[0]);
							Button * theirbutton = otherteam->GetComponent<Button>();
							theirbutton->setStayHovered(false);
							stayOnClick = true;
						}
						else if (buttonType == CHANGE_TEAM_B)
						{
							GameObject * otherteam = GetGameObject()->GetUIGameObjects(helperIndex[0]);
							Button * theirbutton = otherteam->GetComponent<Button>();
							theirbutton->setStayHovered(false);
							stayOnClick = true;
						}
					}
					else if (buttonType == RESUME_GAME)
					{
						isActive = false;

						for (unsigned int i = 0; i < helperIndex.size(); ++i) {
							GameObject * exitObj = GetGameObject()->GetUIGameObjects(helperIndex[i]);
							Button * exitButton = exitObj->GetComponent<Button>();
							exitButton->SetActive(false);
						}
					}

					

				}
				else
				{
					hovered = true;
				}
			}
			else
			{
				hovered = false;
			}
		}
	}
}

void Button::setButtonType()
{
	switch (buttonType)
	{
	case BUTTON_TYPE::HOST:
	{
		eventFunction = StartServer;
		break;
	}
	case BUTTON_TYPE::JOIN:
	{
		eventFunction = JoinServer;
		break;
	}
	case BUTTON_TYPE::PLAY:
	{
		eventFunction = StartGame;
		break;
	}
	case BUTTON_TYPE::CREDITS:
	{
		break;
	}
	case BUTTON_TYPE::EXIT:
	{
		eventFunction = ShutdownGame;
		break;
	}
	case BUTTON_TYPE::RETURN:
	{
		eventFunction = ReturnToMenu;
		break;
	}
	case BUTTON_TYPE::CHANGE_TEAM_A:
	{
		eventFunction = ChangeTeamA;
		break;
	}
	case BUTTON_TYPE::CHANGE_TEAM_B:
	{
		eventFunction = ChangeTeamB;
		break;
	}
	case BUTTON_TYPE::LOBBY:
	{
		eventFunction = StartLobby;
		break;
	}
	}
}