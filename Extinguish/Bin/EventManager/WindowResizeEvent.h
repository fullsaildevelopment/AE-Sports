#pragma once
#include "Event.h"
#include <cstdint>

class WindowResizeEvent : public Event
{
	void Init() {};
public:
	uint16_t h;
	uint16_t w;
};