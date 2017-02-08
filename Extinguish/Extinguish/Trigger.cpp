#include "Trigger.h"

namespace Param
{
	void Trigger::Init(std::string triggerName, bool toggle)
	{
		SetName(triggerName);
		trigger = toggle;
	}

	//if a trigger is set to true, when it's checked, it will toggle it back automatically to false
	bool Trigger::Check()
	{
		bool result = false;

		if (trigger)
		{
			result = true;
			trigger = false;
		}

		return result;
	}

	//getter//
	bool Trigger::GetTrigger() const
	{
		return trigger;
	}

	//setter//
	void Trigger::SetTrigger()
	{
		trigger = true;
	}
}