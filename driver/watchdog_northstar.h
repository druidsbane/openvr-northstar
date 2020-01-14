#pragma once
#include <openvr_driver.h>
#include "driverlog_northstar.h"

#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

void WatchdogThreadFunction();

class CWatchdogDriver_NorthStar : public vr::IVRWatchdogProvider
{
public:
	CWatchdogDriver_NorthStar();
	virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);
	virtual void Cleanup();

private:
	std::thread* m_pWatchdogThread = 0;
};
