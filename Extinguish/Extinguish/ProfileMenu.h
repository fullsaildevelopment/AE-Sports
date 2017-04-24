#pragma once
#include <vector>
#include "Component.h"

class GameObject;

class ProfileMenu : public Component
{
private:
	GameObject* player; //have the player rendering to the side of the menu that pops up. Have its color preview.
	GameObject* name;
	std::vector<GameObject*> statsLabels;
	std::vector<GameObject*> stats;
public:

};