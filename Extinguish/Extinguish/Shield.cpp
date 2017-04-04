#include "Shield.h"

//structors
Shield::Shield()
{

}

//basic
void Shield::Activate()
{

}

void Shield::Deactivate()
{
	PowerUp::Deactivate();
}

std::string Shield::GetName()
{
	return name;
}