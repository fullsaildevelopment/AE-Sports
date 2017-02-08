#include "LoadSceneEvent.h"

void LoadSceneEvent::Init()
{

}

void LoadSceneEvent::Init(std::string sceneName)
{
	SetName(sceneName);
}

//getter//
std::string LoadSceneEvent::GetName()
{
	return name;
}

//setter//
void LoadSceneEvent::SetName(std::string sceneName)
{
	name = sceneName;
}