#pragma once
#include <vector>
#include <string>

class CAkFilePackageLowLevelIOBlocking;

class SoundEngine
{
private:
	CAkFilePackageLowLevelIOBlocking* lowLevelIO;

	//private helper functions
	bool InitSettings();
	void InitBank();
	void InitGameObjects(std::vector<unsigned int> ids, std::vector<std::string> names);
public:
	bool InitSoundEngine(std::vector<unsigned int> ids, std::vector<std::string> names);
	void ProcessAudio();
	void Terminate();
};