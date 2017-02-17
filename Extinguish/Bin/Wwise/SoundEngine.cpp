#include "SoundEngine.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/IBytes.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "AK/SoundEngine/Common/AkModule.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"
#include "AkFilePackageLowLevelIOBlocking.h"
//#include "AK/SoundEngine/Common/AkTypes.h"

#ifndef AK_OPTIMIZED
	#include <AK/Comm/AkCommunication.h>
#endif

//****************************************READ THIS********************************************//
//ids aren't 0 based in wwise. they're 1 based. They're sent in 0 based from engine side though//
//so make sure you + 1 them, or bad things will happen//
//*********************************************************************************************//

// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}
	void FreeHook(void * in_ptr)
	{
		free(in_ptr);
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
	void * VirtualAllocHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwAllocationType,
		DWORD in_dwProtect
	)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}
	void VirtualFreeHook(
		void * in_pMemAddress,
		size_t in_size,
		DWORD in_dwFreeType
	)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
#endif
}

SoundEngine* SoundEngine::singleton = 0;

SoundEngine::SoundEngine()
{

}

SoundEngine::~SoundEngine()
{

}

//TODO: to optimize update, just send a vector of GameObject pointers and then store them in a vector. Cast the pointer to a GameObjectID for the id instead
bool SoundEngine::InitSoundEngine(std::vector<unsigned int> ids, std::vector<std::string> names)
{
	bool result = true;

	lowLevelIO = new CAkFilePackageLowLevelIOBlocking();

	result = InitSettings();

	InitBank();

	InitGameObjects(ids, names);

	//AK::SoundEngine::SetActiveListeners(2, 0);

	return result;
}

void SoundEngine::UpdatePositions(std::vector<DirectX::XMFLOAT3> const & positions, std::vector<DirectX::XMFLOAT3> const & forwards)
{
	for (int i = 0; i < positions.size(); ++i)
	{
		AkSoundPosition soundPos;
		soundPos.SetPosition(positions[i].x, positions[i].y, positions[i].z);
		soundPos.SetOrientation(forwards[i].x, forwards[i].y, forwards[i].z, 0, 1, 0);
		AKRESULT akResult = AK::SoundEngine::SetPosition(i + 1, soundPos);

		if (akResult != AKRESULT::AK_Success)
		{
			int a = 20;
			a += -5;
		}
	}
}

void SoundEngine::UpdateListener(DirectX::XMFLOAT3 const & position, DirectX::XMFLOAT3 const & forward)
{
	AkListenerPosition listener;

	listener.SetPosition(position.x, position.y, position.z);
	listener.SetOrientation(forward.x, forward.y, forward.z, 0, 1, 0);
	AKRESULT akResult = AK::SoundEngine::SetListenerPosition(listener, 0);

	if (akResult != AKRESULT::AK_Success)
	{
		int a = 20;
		a += -5;
	}

	//AK::SoundEngine::SetListenerPosition(listener, index);
}

void SoundEngine::ProcessAudio()
{
	AKRESULT akResult = AK::SoundEngine::RenderAudio();

	if (akResult != AKRESULT::AK_Success)
	{
		int a = 20;
		a += -5;
	}
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
	lowLevelIO->Term();

	//terminate streaming manager
	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}

	//terminate memory manager
	AK::MemoryMgr::Term();

	delete singleton;
	singleton = nullptr;
}

//misc//
SoundEngine* SoundEngine::GetSingleton()
{
	if (!singleton)
	{
		singleton = new SoundEngine();
	}

	return singleton;
}

bool SoundEngine::PostEvent(AkUniqueID eventID, AkGameObjectID gameObjectID)
{
	bool result = true;

	AK::SoundEngine::PostEvent(eventID, gameObjectID);

	return result;
}

//private helper functions//
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

	if (lowLevelIO->Init(devSettings) != AK_Success)
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
	lowLevelIO->SetBasePath(L"../Assets/Soundbanks/");
	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

	AkBankID bankID;
	
	//init bank must be loaded first before anything!
	AKRESULT eResult = AK::SoundEngine::LoadBank(L"Init.bnk", AK_DEFAULT_POOL_ID, bankID);
	eResult = AK::SoundEngine::LoadBank(L"TitansWithSticks.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"Car.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"Human.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"MarkerTest.bnk", AK_DEFAULT_POOL_ID, bankID);
}

void SoundEngine::InitGameObjects(std::vector<unsigned int> ids, std::vector<std::string> names)
{
	for (int i = 0; i < ids.size(); ++i)
	{
		AkGameObjectID id = ids[i] + 1; //wwise doesn't like 0
		const char* name = names[i].c_str();
		AKRESULT akResult = AK::SoundEngine::RegisterGameObj(id, name);

		if (akResult != AKRESULT::AK_Success)
		{
			int a = 20;
			a += -5;
		}
	}
}