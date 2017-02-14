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

bool SoundEngine::InitSoundEngine(std::vector<unsigned int> ids, std::vector<std::string> names)
{
	bool result = true;

	lowLevelIO = new CAkFilePackageLowLevelIOBlocking();

	result = InitSettings();

	InitBank();

	InitGameObjects(ids, names);

	return result;
}

void SoundEngine::UpdatePositions(std::vector<DirectX::XMFLOAT4> const & positions)
{
	for (int i = 0; i < positions.size(); ++i)
	{
		AkSoundPosition soundPos;
		soundPos.SetPosition(positions[i].x, positions[i].y, positions[i].z);
		soundPos.SetOrientation(-1, 0, 0, -1, 0, 0);
		AK::SoundEngine::SetPosition(i, soundPos);
	}
}

void SoundEngine::UpdateListener(DirectX::XMFLOAT4 const & position)
{
	AkListenerPosition listener;

	listener.SetPosition(position.x, position.y, position.z);
	listener.SetOrientation(-1, 0, 0, -1, 0, 0);
	AK::SoundEngine::SetListenerPosition(listener, 0);
	//AK::SoundEngine::SetListenerPosition(listener, index);
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
	eResult = AK::SoundEngine::LoadBank(L"SoundBank.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"Car.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"Human.bnk", AK_DEFAULT_POOL_ID, bankID);
	//eResult = AK::SoundEngine::LoadBank(L"MarkerTest.bnk", AK_DEFAULT_POOL_ID, bankID);
}

void SoundEngine::InitGameObjects(std::vector<unsigned int> ids, std::vector<std::string> names)
{
	for (int i = 0; i < ids.size(); ++i)
	{
		AkGameObjectID id = ids[i];
		AK::SoundEngine::RegisterGameObj(id, names[i].c_str());
	}
}