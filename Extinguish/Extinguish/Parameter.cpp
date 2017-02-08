#include "Parameter.h"

//getters//
std::string Parameter::GetName()
{
	return name;
}

//setters//
void Parameter::SetName(std::string triggerName)
{
	name = triggerName;
}