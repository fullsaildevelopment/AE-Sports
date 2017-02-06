#pragma once
#include <string>
#include "Parameter.h"

namespace Param
{
	class Trigger : public Parameter
	{
	private:
		std::string name;
		bool trigger;
	public:
		void Init(std::string triggerName, bool toggle);
		bool Check() override;

		//setter//
		void SetTrigger(bool toggle);
		void SetName(std::string triggerName);
	};
}