#pragma once
#include "watchdog_northstar.h"

bool g_bExiting = false;


void WatchdogThreadFunction()
{
	// Need to come up with a proper method to detect the headset is connected....b.stastny
	while (!g_bExiting)
	{
		s_logger.DriverLog("Starting WatchDog");
		std::this_thread::sleep_for(std::chrono::seconds(5));

		vr::VRWatchdogHost()->WatchdogWakeUp(vr::ETrackedDeviceClass::TrackedDeviceClass_HMD);
	}
}

CWatchdogDriver_NorthStar::CWatchdogDriver_NorthStar() {

}

vr::EVRInitError CWatchdogDriver_NorthStar::Init(vr::IVRDriverContext* pDriverContext)
{
	s_logger.DriverLog("Starting WatchDog INIT");
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);


	g_bExiting = false;
	m_pWatchdogThread = new std::thread(WatchdogThreadFunction);
	if (!m_pWatchdogThread)
	{
		s_logger.DriverLog("Unable to create watchdog thread\n");
		return vr::VRInitError_Driver_Failed;
	}
	return vr::VRInitError_None;
}

void CWatchdogDriver_NorthStar::Cleanup()
{
	g_bExiting = true;
	if (m_pWatchdogThread)
	{
		m_pWatchdogThread->join();
		delete m_pWatchdogThread;
		m_pWatchdogThread = nullptr;
	}
}
