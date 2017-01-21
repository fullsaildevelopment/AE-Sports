#pragma once

class CAkFilePackageLowLevelIOBlocking;

class SoundEngine
{
private:
	CAkFilePackageLowLevelIOBlocking* lowLevelIO;

	//private helper functions
	bool InitSettings();
	void InitBank();
public:
	bool InitSoundEngine();
	void ProcessAudio();
	void Terminate();
};