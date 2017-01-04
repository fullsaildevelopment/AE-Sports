#pragma once

//This class will be used to make future components such as renderer, AI, etc.

class Component
{
private:

public:
	//none of these necessarily need to be used, they're just there in case and for some unity
	virtual void Init() {};
	virtual void Update() {};
	virtual void Shutdown() {};
};