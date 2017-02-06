#pragma once
#include <vector>
#include <string>
#include "Wwise_IDs.h"

class CAkFilePackageLowLevelIOBlocking;
//class AkUniqueID;
//class AkGameObjectID;

class SoundEngine
{
private:
	CAkFilePackageLowLevelIOBlocking* lowLevelIO;

	static SoundEngine* singleton;

	//private helper functions
	bool InitSettings();
	void InitBank();
	void InitGameObjects(std::vector<unsigned int> ids, std::vector<std::string> names);

public:
	SoundEngine();
	~SoundEngine();

	//basic//
	bool InitSoundEngine(std::vector<unsigned int> ids, std::vector<std::string> names);
	void ProcessAudio();
	void Terminate();

	//misc
	static SoundEngine* GetSingleton();
	bool PostEvent(AkUniqueID eventID, AkGameObjectID gameObjectID);
	//bool PlaySpearSound();
	//bool PlayWalkingSound();
	//bool StopWalkingSound();
};