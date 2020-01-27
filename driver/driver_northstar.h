#pragma once
// derived from Valve reference driver
//

#include <algorithm>
#include <chrono>
#include <mutex>
#include <openvr_driver.h>
#include <thread>
#include <vector>

#if defined( WIN32)
#include <windows.h>
#endif

#include "driverlog_northstar.h"
#include "deformation_northstar.h"

//meh
using namespace std::chrono;
using namespace vr;


// keys for use with the settings API
static const char * const k_pch_Sample_Section = "driver_northstar";

// static for windows api to write to
static long s_monitorStart;
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CNorthStarDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
{
public:
    CNorthStarDeviceDriver();

    virtual ~CNorthStarDeviceDriver();
    virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
    virtual void Deactivate();
    virtual void EnterStandby();

    void* GetComponent(const char *pchComponentNameAndVersion);
    virtual void PowerOff();
    virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);
    virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight);
    virtual bool IsDisplayOnDesktop();
    virtual bool IsDisplayRealDisplay();
    virtual void GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight);
    virtual void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight);
    virtual void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom);
    virtual DistortionCoordinates_t ComputeDistortion(EVREye eEye, float fU, float fV);
    virtual DriverPose_t GetPose();
    void RunFrame();

    std::string GetSerialNumber() const { return m_sSerialNumber; }

    //void Log(char* name, Matrix4x4 _in) {
        //s_logger.DriverLog("%s : \n", name);
        //s_logger.DriverLog("%f %f %f %f\n", _in.m00, _in.m01, _in.m02, _in.m03);
        //s_logger.DriverLog("%f %f %f %f\n", _in.m10, _in.m11, _in.m12, _in.m13);
        //s_logger.DriverLog("%f %f %f %f\n", _in.m20, _in.m21, _in.m22, _in.m23);
        //s_logger.DriverLog("%f %f %f %f\n", _in.m30, _in.m31, _in.m32, _in.m33);
    //}


private:

    // StreamVR handles
    vr::TrackedDeviceIndex_t m_unObjectId;
    vr::PropertyContainerHandle_t m_ulPropertyContainer;

    // in ms
    int m_headPoseTimeOffset;

    // Positional component that moves the device in device-space to the center
    // eye position.
    Vector3 m_centerEyeFromDevicePosition;

    // Rotational component that rotates from device-space to center-eye space.
    Vector3 m_centerEyeFromDeviceRotationEuler;

    // the most recent headpose, probably don't need this to be a member anymore
    // being a local variable should be okay, come back and clean up
    DriverPose_t m_HeadPose;

    // baisic information about the device display
    int32_t m_nWindowX;
    int32_t m_nWindowY;
    int32_t m_nWindowWidth;
    int32_t m_nWindowHeight;
    int32_t m_nRenderWidth;
    int32_t m_nRenderHeight;

    int32_t m_nVendorId;   // we need to read this from the hardware
    int32_t m_nProductId;   // we need to read this from the hardware
    std::string m_sSerialNumber; // we need to read this from the hardware
    std::string m_sModelNumber; // we need to read this from the hardware

    bool m_nDirectModeEnabled;

    float m_flSecondsFromVsyncToPhotons;
    float m_flDisplayFrequency;
    float m_flIPD;  // currnet IPD

    // eye tracker device offsets
    Vector3 m_EyeTrackerOffset;
    Vector3 m_EyeTrackerRotation;

    // data to enable building the reflector ray tracer
    // used to properly distort the rendering.
    OpticalSystem  m_leftEye;
    OpticalSystem  m_rightEye;

    // logging
    bool m_Verbose;

    // flag that the Driver is shutting down and signals all the
    // child threads to exit
    bool m_bExiting;

    // Do we have valid eye data?  We do not want to be adjust
    // the head pose unless the eye data is correct.
    bool m_hasEyeData;

    // the last left / right eye smoothed position in
    // steam VR space
    Vector3 m_LastLeftEye;
    Vector3 m_LastRightEye;

    // set in config file, the max eye in eye tracking before
    // we throw out the current eye data and restart the smoothing
    // process
    float m_MaxEyeTrackingError;

    // the number of points we smooth the eye position over.
    int m_EyeTrackingSmoothing;

    // set to true if we want to constantly update the detected IPD
    bool m_recalcIPD;

    // number of UVs that have been read between eye tracking collection
    // events, we never want to set a new mesh deformation unless the last
    // defomation has been fully read, note that even while a mesh is being
    // read we do update the calibataion data so the mesh is always as accurate
    // as possible, this just avoids us swamping steamVR with update requests
    int m_uvReadCount;

    // the Eye offests
    Vector3 m_LEye;
    Vector3 m_REye;


    //  development controls, allows setting the solver iterations
    //  from the config file in case we run into any issues.
    //  init is the first call to the solver and opt is all later calls
    int m_initSolverIters;
    int m_optSolverIters;

    #if defined( WIN32)
    static BOOL CALLBACK CNorthStarDeviceDriver::MyInfoEnumProc(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
    #endif
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CNorthStarDriver : public IServerTrackedDeviceProvider
{
public:
    CNorthStarDriver()
        : m_pNullHmdLatest(NULL)
    {
    }
    virtual ~CNorthStarDriver();

    virtual EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    virtual void Cleanup();
    virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }
    virtual void RunFrame();
    virtual bool ShouldBlockStandbyMode() { return false; }
    virtual void EnterStandby()
    {
        if (m_pNullHmdLatest) {
            m_pNullHmdLatest->EnterStandby();
        }
    }
    virtual void LeaveStandby() {
        // not sure what we have to make the headset do here..
    }

private:

    CNorthStarDeviceDriver * m_pNullHmdLatest;

};