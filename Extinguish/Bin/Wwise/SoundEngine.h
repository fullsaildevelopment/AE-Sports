#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "Wwise_IDs.h"

class CAkFilePackageLowLevelIOBlocking;
//class AkUniqueID;
//class AkGameObjectID;

//TODO: Probably don't need this to be a singleton anymore

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
	void UpdatePositions(std::vector<DirectX::XMFLOAT3> const & positions, std::vector<DirectX::XMFLOAT3> const & forwards);
	void UpdateListener(DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & forward);
	void ProcessAudio();
	void Terminate();

	//getters//
	static SoundEngine* GetSingleton();

	//misc
	bool PostEvent(AkUniqueID eventID, AkGameObjectID gameObjectID);
};