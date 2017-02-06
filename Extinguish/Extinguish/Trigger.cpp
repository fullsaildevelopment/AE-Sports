#include "Trigger.h"

namespace Param
{
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
}