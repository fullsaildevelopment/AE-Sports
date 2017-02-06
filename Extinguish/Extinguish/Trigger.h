#pragma once
#include "Parameter.h"

namespace Param
{
	class Trigger : public Parameter
	{
	private:
		bool trigger;
	public:
		bool Check() override;

		//setter//
		void SetTrigger(bool toggle);
	};
}