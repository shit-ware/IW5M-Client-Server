// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: Entry points for SteamAPI
//
// Initial author: NTAuthority
// Started: 2010-11-21
// ==========================================================

#include "stdinc.h"
#include <Shlwapi.h>
#include "GSServer.h"
#include "GSClient.h"
//#include "Callbacks.h"
#include <fstream>
#include <sstream>
#include <string>

void NPA_KickClient(NPID npID, const char* reason)
{
	// determine the clientnum
	char* clientAddress = (char*)0x4B4D590;

	if (g_isDedicated)
	{
		clientAddress = (char*)0x49EB690;
	}

	Trace("NPA", "KickClient commanded to kick %llx for %s", npID, reason);

	int i = 0;
	bool found = false;

	int* count = (int*)0x4B4D58C;

	if (g_isDedicated)
	{
		count = (int*)0x49EB68C;
	}

	Trace("NPA", "%d clients", *count);

	for (i = 0; i < *count; i++)
	{
		if (*clientAddress >= 3)
		{
			int64_t clientID = *(int64_t*)(clientAddress + ((g_isDedicated) ? 283320 : 283328));

			Trace("NPA", "client %d is %llx", i, clientID);

			if (clientID == npID)
			{
				Trace("NPA", "found him!");

				found = true;
				break;
			}
		}

		clientAddress += (!g_isDedicated) ? 493200 : 493192;
	}

	if (!found)
	{
		Trace("NPA", "found nobody, returning");
		return;
	}

	Trace("NPA", "execing dropClient %d \"%s\"", i, reason);
	Cbuf_AddText(0, va("dropClient %d \"%s\"\n", i, reason));
}

void Friends_StatsCB(NPAsync<NPFriendResult>* async)
{
	NPFriendResult* result = async->GetResult();
	Com_Printf(0, "Loaded %i friends.\n", result->numResults);
}

void NP_LogCB(const char* message)
{
	Com_Printf(0, va("[RepZ auth] %s", message));
}

void NPA_StateSet(EExternalAuthState result);
void SteamProxy_Init();
void SteamProxy_RunFrame();
void SteamProxy_DoThatTwinklyStuff();
void PatchIW5_ConsoleTitle();

// Steam API code
extern "C"
{
	__declspec(dllexport) HSteamPipe __cdecl GetHSteamPipe()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamUser __cdecl GetHSteamUser()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamPipe __cdecl SteamAPI_GetHSteamPipe()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamUser __cdecl SteamAPI_GetHSteamUser()
	{
		return NULL;
	}
	__declspec(dllexport) const char *__cdecl SteamAPI_GetSteamInstallPath()
	{
		return NULL;
	}

	__declspec(dllexport) bool __cdecl SteamAPI_Init()
	{
		//TODO: fix this for iw5m
		//DisableOldVersions();
		//checkSize();

		NP_SetLogCallback(NP_LogCB);
		NP_Init();

		NP_RegisterEACallback(NPA_StateSet);

		//3036 = live np
		//3037 = dev np
		#ifdef DEVMODE
		if (!NP_Connect(MASTER_SERVER, 3037))
		#else
		if (!NP_Connect(MASTER_SERVER, 3036))
		#endif
		{
			Com_Error(1, "Could not connect to NP server at " MASTER_SERVER);
			return false;
		}

		SteamProxy_Init();
		SteamProxy_DoThatTwinklyStuff();

		NPAuthenticateResult* result;

		NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithToken(Auth_GetSessionID());
		result = async->Wait();

		if (result->result != AuthenticateResultOK)
		{
			switch (result->result)
			{
				case AuthenticateResultBadDetails:
					Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- bad details.");
					break;
				case AuthenticateResultAlreadyLoggedIn:
					Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- already logged in.");
					break;
				case AuthenticateResultBanned:
					Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- banned.");
					break;
				case AuthenticateResultUnknown:
					Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- unknown error.");
					break;
			}
		}

		NP_RegisterKickCallback(NPA_KickClient);
		NPAsync<NPFriendResult>* friendReq = NP_GetFriends();
		friendReq->SetCallback(Friends_StatsCB, NULL);

		//LoadLibrary("aci2.dll");

		GSClient_Init();

		return true;
	}

	__declspec(dllexport) bool __cdecl SteamAPI_InitSafe()
	{
		return true;
	}

	__declspec(dllexport) char __cdecl SteamAPI_RestartApp()
	{
		return 1;
	}

	__declspec(dllexport) char __cdecl SteamAPI_RestartAppIfNecessary()
	{
		return 0;
	}

	__declspec(dllexport) void __cdecl SteamAPI_RegisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
	{
		Trace("S_API", "RegisterCallResult for call ID %d", APICall);
		//Callbacks::RegisterResult(pResult, APICall);
		CSteamBase::RegisterCallResult(APICall, pResult);
	}

	__declspec(dllexport) void __cdecl SteamAPI_RegisterCallback( CCallbackBase *pCallback, int iCallback )
	{
		CSteamBase::RegisterCallback(pCallback, iCallback);
		//Callbacks::Register(pCallback, iCallback);
	}

	__declspec(dllexport) void __cdecl SteamAPI_RunCallbacks()
	{
		CSteamBase::RunCallbacks();
		NP_RunFrame();
		SteamProxy_RunFrame();
		GSClient_RunFrame();
		//Callbacks::Run();
	}
	__declspec(dllexport) void __cdecl SteamAPI_SetMiniDumpComment( const char *pchMsg )
	{
	}

	__declspec(dllexport) bool __cdecl SteamAPI_SetTryCatchCallbacks( bool bUnknown )
	{
		return bUnknown;
	}
	__declspec(dllexport) void __cdecl SteamAPI_Shutdown()
	{
	}
	__declspec(dllexport) void __cdecl SteamAPI_UnregisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
	{
	}
	__declspec(dllexport) void __cdecl SteamAPI_UnregisterCallback( CCallbackBase *pCallback, int iCallback )
	{
	}

	__declspec(dllexport) void __cdecl SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID )
	{
	}

	__declspec(dllexport) ISteamApps* __cdecl SteamApps()
	{
		Trace("S_API", "SteamApps");
		return (ISteamApps*)CSteamBase::GetInterface(INTERFACE_STEAMAPPS004);
		//return (ISteamApps*)g_pSteamAppsEmu;
	}
	__declspec(dllexport) ISteamClient* __cdecl SteamClient()
	{
		Trace("S_API", "SteamClient");
		return NULL;
		//return (ISteamClient*)g_pSteamClientEmu;
	}
	__declspec(dllexport) ISteamContentServer* __cdecl SteamContentServer()
	{
		return NULL;
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamContentServerUtils()
	{
		return NULL;
	}

	__declspec(dllexport) bool __cdecl SteamContentServer_Init( unsigned int uLocalIP, unsigned short usPort )
	{
		return NULL;
	}

	__declspec(dllexport) void __cdecl SteamContentServer_RunCallbacks()
	{
	}

	__declspec(dllexport) void __cdecl SteamContentServer_Shutdown()
	{
	}

	__declspec(dllexport) ISteamFriends* __cdecl SteamFriends()
	{
		//Trace("S_API", "SteamFriends");
		return (ISteamFriends*)CSteamBase::GetInterface(INTERFACE_STEAMFRIENDS009);
		//return (ISteamFriends*)g_pSteamFriendsEmu;
	}

	__declspec(dllexport) ISteamGameServer* __cdecl SteamGameServer()
	{
		return (ISteamGameServer*)CSteamBase::GetInterface(INTERFACE_STEAMGAMESERVER010);
		//return (ISteamGameServer*)g_pSteamGameServerEmu;
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamGameServerUtils()
	{
		return (ISteamUtils*)CSteamBase::GetInterface(INTERFACE_STEAMUTILS005);
	}

	__declspec(dllexport) bool __cdecl SteamGameServer_BSecure()
	{
		return true;
	}

	__declspec(dllexport) HSteamPipe __cdecl SteamGameServer_GetHSteamPipe()
	{
		return NULL;
	}

	__declspec(dllexport) HSteamUser __cdecl SteamGameServer_GetHSteamUser()
	{
		return NULL;
	}

	__declspec(dllexport) int32 __cdecl SteamGameServer_GetIPCCallCount()
	{
		return NULL;
	}

	__declspec(dllexport) uint64 __cdecl SteamGameServer_GetSteamID()
	{
		return NULL;
	}

	static bool steamGS_Inited = false;

	__declspec(dllexport) bool __cdecl SteamGameServer_Init( uint32 unIP, uint16 usPort, uint16 usGamePort, uint16 usSpectatorPort, uint16 usQueryPort, EServerMode eServerMode, const char *pchGameDir, const char *pchVersionString )
	{
		if (isClient)
		{
			return true;
		}

		if (steamGS_Inited)
		{
			return true;
		}

		NP_SetLogCallback(NP_LogCB);
		NP_Init();

		//3036 = live np
		//3037 = dev np
		#ifdef DEVMODE
				if (!NP_Connect(MASTER_SERVER, 3037))
		#else
				if (!NP_Connect(MASTER_SERVER, 3036))
		#endif
		{
			Com_Printf(0, "Could not connect to NP server at " MASTER_SERVER ".");
			return false;
		}

		NPAuthenticateResult* result;
		//iw5m doesn't have Dvar_RegisterString ;(
		const char* licenseFile = GetLicenseFile();

		std::ifstream in(licenseFile);
		std::string licenseKey((std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>());

		if (licenseKey.length() == 0) {
			Com_Error(0, "License file: %s could not be found.", licenseFile);
			return false;
		}

		NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey(licenseKey.c_str());
		result = async->Wait();

		if (result->result != AuthenticateResultOK)
		{
			switch (result->result)
			{
			case AuthenticateResultBadDetails:
				Com_Printf(0, "Could not authenticate to NP server at " MASTER_SERVER " -- bad details.");
				return false;
			case AuthenticateResultAlreadyLoggedIn:
				Com_Printf(0, "Could not authenticate to NP server at " MASTER_SERVER " -- already logged in");
				return false;
			case AuthenticateResultBanned:
				Com_Printf(0, "Could not authenticate to NP server at " MASTER_SERVER " -- banned.");
				return false;
			case AuthenticateResultUnknown:
				Com_Printf(0, "Could not authenticate to NP server at " MASTER_SERVER " -- unknown error.");
				return false;
			}
		}

		NP_RegisterKickCallback(NPA_KickClient);
		
		steamGS_Inited = true;

		return GSServer_Init(usGamePort, usQueryPort);

		//return true;
	}

	__declspec(dllexport) bool __cdecl SteamGameServer_InitSafe( uint32 unIP, uint16 usPort, uint16 usGamePort, EServerMode eServerMode, int nGameAppId, const char *pchGameDir, const char *pchVersionString, unsigned long dongs )
	{
		return true;
	}

	boolean doneInitStuff = false;
	__declspec(dllexport) void __cdecl SteamGameServer_RunCallbacks()
	{
		NP_RunFrame();
		GSServer_RunFrame();
		if (!doneInitStuff) {
			PatchIW5_ConsoleTitle();
		}
		
		if (g_isDedicated)
		{
			CSteamBase::RunCallbacks();
		}
	}

	__declspec(dllexport) void __cdecl SteamGameServer_Shutdown()
	{
	}

	__declspec(dllexport) ISteamMasterServerUpdater* __cdecl SteamMasterServerUpdater()
	{
		return (ISteamMasterServerUpdater*)CSteamBase::GetInterface(INTERFACE_STEAMMASTERSERVERUPDATER001);
		//return (ISteamMasterServerUpdater*)g_pSteamMasterServerUpdaterEmu;
	}

	__declspec(dllexport) ISteamMatchmaking* __cdecl SteamMatchmaking()
	{
		//Trace("S_API", "SteamMatchmaking");
		return (ISteamMatchmaking*)CSteamBase::GetInterface(INTERFACE_STEAMMATCHMAKING008);
		//return (ISteamMatchmaking*)g_pSteamMatchMakingEmu;
	}

	__declspec(dllexport) ISteamMatchmakingServers* __cdecl SteamMatchmakingServers()
	{
		return (ISteamMatchmakingServers*)CSteamBase::GetInterface(INTERFACE_STEAMMATCHMAKINGSERVERS002);
	}

	__declspec(dllexport) ISteamNetworking* __cdecl SteamGameServerNetworking()
	{
		//Trace("S_API", "SteamNetworking");
		return (ISteamNetworking*)CSteamBase::GetInterface(INTERFACE_STEAMNETWORKING005);
	}

	__declspec(dllexport) ISteamNetworking* __cdecl SteamNetworking()
	{
		//Trace("S_API", "SteamNetworking");
		return (ISteamNetworking*)CSteamBase::GetInterface(INTERFACE_STEAMNETWORKING005);
	}

	__declspec(dllexport) void* __cdecl SteamRemoteStorage()
	{
		//return g_pSteamRemoteStorageEmu;
		return CSteamBase::GetInterface(INTERFACE_STEAMREMOTESTORAGE002);
	}

	__declspec(dllexport) ISteamUser* __cdecl SteamUser()
	{
		//return (ISteamUser*)g_pSteamUserEmu;
		//Trace("S_API", "SteamUser");
		return (ISteamUser*)CSteamBase::GetInterface(INTERFACE_STEAMUSER016);
	}

	__declspec(dllexport) ISteamUserStats* __cdecl SteamUserStats()
	{
		//return (ISteamUserStats*)g_pSteamUStatsEmu;
		Trace("S_API", "SteamUserStats");
		return (ISteamUserStats*)CSteamBase::GetInterface(INTERFACE_STEAMUSERSTATS010);
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamUtils()
	{
		//return (ISteamUtils*)g_pSteamUtilsEmu;
		Trace("S_API", "SteamUtils");
		return (ISteamUtils*)CSteamBase::GetInterface(INTERFACE_STEAMUTILS005);
	}

	__declspec(dllexport) HSteamUser __cdecl Steam_GetHSteamUserCurrent()
	{
		return NULL;
	}

	__declspec(dllexport) void __cdecl Steam_RegisterInterfaceFuncs( void *hModule )
	{

	}

	__declspec(dllexport) void __cdecl Steam_RunCallbacks( HSteamPipe hSteamPipe, bool bGameServerCallbacks )
	{

	}

	__declspec(dllexport) void *g_pSteamClientGameServer = NULL;
}
