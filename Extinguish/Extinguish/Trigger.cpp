#include "Trigger.h"

namespace Param
{
	void Trigger::Init(std::string triggerName, bool toggle)
	{
		name = triggerName;
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

	void Trigger::SetTrigger(bool toggle)
	{
		trigger = toggle;
	}

	void Trigger::SetName(std::string triggerName)
	{
		name = triggerName;
	}
}