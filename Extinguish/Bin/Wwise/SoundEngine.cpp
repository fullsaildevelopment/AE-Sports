#include "SoundEngine.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/IBytes.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "AK/SoundEngine/Common/AkModule.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"

#ifndef AK_OPTIMIZED
	#include <AK/Comm/AkCommunication.h>
#endif

bool SoundEngine::InitSoundEngine()
{
	bool result = true;

	result = InitSettings();

	return result;
}

void SoundEngine::ProcessAudio()
{
	AK::SoundEngine::RenderAudio();
}

void SoundEngine::Terminate()
{
	//terminate communications with authoring tool... Must be terminated before other modules
#ifndef AK_OPTIMIZED
	AK::Comm::Term();
#endif

	//terminate music engine
	AK::MusicEngine::Term();

	//terminate sound engine
	AK::SoundEngine::Term();

	//terminate low level io
	lowLevelIO.Term();

	//terminate streaming manager
	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}

	//terminate memory manager
	AK::MemoryMgr::Term();
}

//private helper functions
bool SoundEngine::InitSettings()
{
	bool result = true;

	//init memory manager
	AkMemSettings memSettings;
	memSettings.uMaxNumPools = 20;

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		result = false;
	}

	//create streaming manager
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	if (!AK::StreamMgr::Create(stmSettings))
	{
		result = false;
	}

	AkDeviceSettings devSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(devSettings);

	if (lowLevelIO.Init(devSettings) != AK_Success)
	{
		result = false;
	}

	//init sound engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		result = false;
	}

	//init music engine
	AkMusicSettings musicSettings;
	AK::MusicEngine::GetDefaultInitSettings(musicSettings);

	if (AK::MusicEngine::Init(&musicSettings) != AK_Success)
	{
		result = false;
	}

	//initialize communcation between wwise authoring app and engine (not in release mode though)
#ifndef AK_OPTIMIZED
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);

	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		result = false;
	}
#endif

	return result;
}

void SoundEngine::InitBank()
{
	lowLevelIO.SetBasePath(AKTEXT("../../../samples/IntegrationDemo/WwiseProject/GeneratedSoundBanks/Windows/"));
	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

	AkBankID bankID;
	
	//init bank must be loaded first before anything!
	AKRESULT eResult = AK::SoundEngine::LoadBank(L"Init.bnk", AK_DEFAULT_POOL_ID, bankID);
	eResult = AK::SoundEngine::LoadBank(L"Car.bnk", AK_DEFAULT_POOL_ID, bankID);
	eResult = AK::SoundEngine::LoadBank(L"Human.bnk", AK_DEFAULT_POOL_ID, bankID);
	eResult = AK::SoundEngine::LoadBank(L"MarkerTest.bnk", AK_DEFAULT_POOL_ID, bankID);

}