
#include <openvr_driver.h>
#include "driverlog_northstar.h"
#include "driver_northstar.h"
#include "watchdog_northstar.h"

#if defined( _WINDOWS )
#include <windows.h>
#endif

using namespace vr;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C"
#else
#error "Unsupported Platform."
#endif

static CWatchdogDriver_NorthStar g_watchdogDriverNorthStar;
static CNorthStarDriver g_serverDriverNorthStar;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
HMD_DLL_EXPORT void* HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
    if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
    {
        return &g_serverDriverNorthStar;
    }
    if (0 == strcmp(IVRWatchdogProvider_Version , pInterfaceName))
    {
        return &g_watchdogDriverNorthStar;
    }

    if (pReturnCode)
        *pReturnCode = VRInitError_Init_InterfaceNotFound;
        return NULL;
}


// we don't do anything here yet...
BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:

    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:

    case DLL_PROCESS_DETACH:

        break;
    };
    return TRUE;
}
