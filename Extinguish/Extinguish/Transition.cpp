#include "Transition.h"
#include "Parameter.h"

void Transition::Update()
{
	//loop through all condition
	//if one is met, go to animation it's pointing to
	for (int i = 0; i < conditions.size(); ++i)
	{
		conditions[i]->Check();
	}
}

void Transition::AddCondition(Parameter* parameter)
{
	conditions.push_back(parameter);
}