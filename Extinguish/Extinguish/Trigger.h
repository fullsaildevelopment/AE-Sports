#pragma once
#include <string>
#include "Parameter.h"

namespace Param
{
	class Trigger : public Parameter
	{
	private:
		bool trigger;
	public:
		void Init(std::string triggerName, bool toggle);
		bool Check() override; //check will eat bool if true

		//getter//
		bool GetTrigger() const; //get trigger will not harm bool

		//setter//
		void SetTrigger();
	};
}