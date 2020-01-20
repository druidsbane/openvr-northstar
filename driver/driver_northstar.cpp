#include "driver_northstar.h"
#include "utility_northstar.h"
#include "driverlog_northstar.h"
#include <chrono>
#include <openvr_driver.h>

// NORTHSTAR DRIVER;

// this has to be a funky windows call back function and this is a placeholder for convience for dev
// work, once we have proper firmware data set and ability to power the headset on / off via software
// this needs to be done differently.
BOOL CALLBACK CNorthStarDeviceDriver::MyInfoEnumProc(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	bool value = GetMonitorInfo(hMon, &info);
	s_logger.DriverLog("NorthStar::Monitors we found:  %s  %d %d %d %d \n", info.szDevice, info.rcMonitor.left, info.rcMonitor.right, info.rcMonitor.top, info.rcMonitor.bottom);

	if ((info.rcMonitor.right - info.rcMonitor.left) == 2880 && ((info.rcMonitor.bottom - info.rcMonitor.top) == 1600)) {
		s_monitorStart = info.rcMonitor.left;
	}

	return value;
}


CNorthStarDeviceDriver::CNorthStarDeviceDriver()
{

	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog(R"(  _        _______  _______ _________          _______ _________ _______  _______  )"); s_logger.Endl();
	s_logger.DriverLog(R"( | \    /||  ___  ||  ____ \\__   __/|\     /|(  ____ \\__   __/(  ___  )(  ____ ) )"); s_logger.Endl();
	s_logger.DriverLog(R"( |  \  | || |   | || |    ||   | |   | )   ( || (    \/   ) (   | (   ) || (    )| )"); s_logger.Endl();
	s_logger.DriverLog(R"( |   \ | || |   | || |____||   | |   | (___) || (_____    | |   | (___) || (____)| )"); s_logger.Endl();
	s_logger.DriverLog(R"( | |\ \| || |   | ||     __/   | |   |  ___  |(_____  )   | |   |  ___  ||     __) )"); s_logger.Endl();
	s_logger.DriverLog(R"( | | \   || |   | || /\ \      | |   | (   ) |      ) |   | |   | (   ) || (\ (    )"); s_logger.Endl();
	s_logger.DriverLog(R"( | |  \  || |___| || | \ \__   | |   | )   ( |/\____) |   | |   | )   ( || ) \ \__ )"); s_logger.Endl();
	s_logger.DriverLog(R"( |/    |_||_______||/   \__/   |_|   |/     \|\_______)   )_(   |/     \||/   \__/ )"); s_logger.Endl();
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("                                 By   Leap Motion \n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");
	s_logger.DriverLog("\n");


	m_initSolverIters = 0;
	m_optSolverIters = 0;
	m_uvReadCount = 0;

	EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, 0);

	s_logger.SetLogger(vr::VRDriverLog());
	m_leftEye.SetLogger(&s_logger);
	m_rightEye.SetLogger(&s_logger);

	m_headPoseTimeOffset = 0;
	m_bExiting = false;
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

	m_Verbose = vr::VRSettings()->GetBool("driver_northstar", "verbose");
	s_logger.DriverLog("NorthStar:: Verbose Mode %d\n", m_Verbose);

	m_flIPD = vr::VRSettings()->GetFloat("driver_northstar", "ipd");
	//vr::VRSettings()->SetFloat(k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float, m_flIPD);
	//vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD);
	s_logger.DriverLog("MorthStar:: Initial IPD %f\n", m_flIPD);

	m_sSerialNumber = "1024";
	//vr::VRSettings()->SetString(k_pch_Sample_Section, k_pch_Null_SerialNumber_String, m_sSerialNumber.c_str());
	//vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());

	m_sModelNumber = "0";
	//vr::VRSettings()->SetString(k_pch_Sample_Section, k_pch_Null_ModelNumber_String, m_sModelNumber.c_str());
	//vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());

	m_nDirectModeEnabled = false;
	//vr::VRSettings()->SetBool(k_pch_Sample_Section, k_pch_DirectMode_Enable_Bool, m_nDirectModeEnabled);
	//vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false); // Should be false.

	m_nVendorId = 0xD041;
	//vr::VRSettings()->SetInt32(k_pch_Sample_Section, k_pch_DirectMode_EdidVid_Int32, m_nVendorId);
	//vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_EdidVendorID_Int32, m_nVendorId);

	m_nProductId = 0;
	//vr::VRSettings()->SetInt32(k_pch_Sample_Section, k_pch_DirectMode_EdidPid_Int32, m_nProductId);
	//vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_EdidProductID_Int32, m_nProductId);

	m_nWindowX = vr::VRSettings()->GetInt32("driver_northstar", "headsetwindowX");
	if (m_nWindowX < 0) {
		m_nWindowX = s_monitorStart;
	}
	s_logger.DriverLog("NorthStar:: Window Location of NorthStar Headset X is  %i\n", m_nWindowX);


	// avoid "not fullscreen" warnings from vrmonitor
	//vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, true);

	// this should for most cases be zero, but we will leave in the option to change it,
	// for anyone that does something funky
	m_nWindowY = vr::VRSettings()->GetInt32("driver_northstar", "headsetwindowY");
	s_logger.DriverLog("NorthStar:: Window Location of NorthStar Headset Y is  %i\n", m_nWindowY);

	// we can auto detect and set these but for now leave them in the config file
	// because they are not changing and this gives us flexibility
	m_nWindowWidth = vr::VRSettings()->GetInt32("driver_northstar", "headsetwindowidth");
	s_logger.DriverLog("NorthStar:: Window Width %i\n", m_nWindowWidth);

	m_nWindowHeight = vr::VRSettings()->GetInt32("driver_northstar", "headsetwindowheight");
	s_logger.DriverLog("NorthStar:: Window Height %i\n", m_nWindowHeight);

	m_nRenderWidth = vr::VRSettings()->GetInt32("driver_northstar", "headsetrenderwidth");
	s_logger.DriverLog("NorthStar:: Window Render Width %i\n", m_nRenderWidth);

	m_nRenderHeight = vr::VRSettings()->GetInt32("driver_northstar", "headsetrenderheight");
	s_logger.DriverLog("NorthStar:: Window Render Height %i\n", m_nRenderHeight);

	m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat("driver_northstar", "photonlatency");
	s_logger.DriverLog("NorthStar:: Photon Latency %g\n", m_flSecondsFromVsyncToPhotons);
	//vr::VRSettings()->SetFloat(k_pch_Sample_Section, k_pch_Null_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons);

	m_flDisplayFrequency = vr::VRSettings()->GetFloat("driver_northstar", "headsetfrequency");
	s_logger.DriverLog("NorthStar:: Display Frequency %f\n", m_flDisplayFrequency);

	// convert to leap required units
	m_headPoseTimeOffset = vr::VRSettings()->GetInt32("driver_northstar", "headposetimeoffset") * 1000;
	s_logger.DriverLog("NorthStar:: Head Pose Time Offset %f\n", m_headPoseTimeOffset);

	m_centerEyeFromDeviceRotationEuler.x = vr::VRSettings()->GetFloat("driver_northstar", "used_devicerotation_x");
	s_logger.DriverLog("NorthStar:: Used Device Rotation X %f\n", m_centerEyeFromDeviceRotationEuler.x);

	m_centerEyeFromDeviceRotationEuler.y = vr::VRSettings()->GetFloat("driver_northstar", "used_devicerotation_y");
	s_logger.DriverLog("NorthStar:: Used Device Rotation Y %f\n", m_centerEyeFromDeviceRotationEuler.y);

	m_centerEyeFromDeviceRotationEuler.z = vr::VRSettings()->GetFloat("driver_northstar", "used_devicerotation_z");
	s_logger.DriverLog("NorthStar:: Used Device Rotation Z %f\n", m_centerEyeFromDeviceRotationEuler.z);

	m_centerEyeFromDevicePosition.x = vr::VRSettings()->GetFloat("driver_northstar", "deviceheadoffset_x");
	s_logger.DriverLog("NorthStar:: Device Head Offset X %f\n", m_centerEyeFromDevicePosition.x);

	m_centerEyeFromDevicePosition.y = vr::VRSettings()->GetFloat("driver_northstar", "deviceheadoffset_y");
	s_logger.DriverLog("NorthStar:: Device Head Offset Y %f\n", m_centerEyeFromDevicePosition.y);

	m_centerEyeFromDevicePosition.z = vr::VRSettings()->GetFloat("driver_northstar", "deviceheadoffset_z");
	s_logger.DriverLog("NorthStar:: Device Head Offset Z %f\n", m_centerEyeFromDevicePosition.z);

	m_initSolverIters = vr::VRSettings()->GetInt32("driver_northstar", "initsolveriters");
	s_logger.DriverLog("NorthStar:: Initial Solver Iterations %d\n", m_initSolverIters);

	m_optSolverIters = vr::VRSettings()->GetInt32("driver_northstar", "optsolveriters");
	s_logger.DriverLog("NorthStar:: Optmized Solver Iterations %d\n", m_optSolverIters);

	m_leftEye.setiters(m_initSolverIters, m_optSolverIters);
	m_rightEye.setiters(m_initSolverIters, m_optSolverIters);

	m_LEye.x = vr::VRSettings()->GetFloat("leftEye", "eyePosition_x");
	m_LEye.y = vr::VRSettings()->GetFloat("leftEye", "eyePosition_y");
	m_LEye.z = vr::VRSettings()->GetFloat("leftEye", "eyePosition_z");
	m_leftEye.UpdateEyePosition(m_LEye);

	m_REye.x = vr::VRSettings()->GetFloat("rightEye", "eyePosition_x");
	m_REye.y = vr::VRSettings()->GetFloat("rightEye", "eyePosition_y");
	m_REye.z = vr::VRSettings()->GetFloat("rightEye", "eyePosition_z");
	m_rightEye.UpdateEyePosition(m_REye);

	m_HeadPose = { 0.f };
	m_HeadPose.poseIsValid = true;
	m_HeadPose.result = TrackingResult_Running_OK;
	m_HeadPose.deviceIsConnected = true;
	m_HeadPose.poseTimeOffset = 0.f;
	m_HeadPose.shouldApplyHeadModel = false;

	//  these are in meters
	m_HeadPose.vecPosition[0] = 0.f;
	m_HeadPose.vecPosition[1] = 0.f;
	m_HeadPose.vecPosition[2] = 0.f;

	//  m/s
	m_HeadPose.vecVelocity[0] = 0.f;
	m_HeadPose.vecVelocity[1] = 0.f;
	m_HeadPose.vecVelocity[2] = 0.f;

	// ident
	m_HeadPose.qWorldFromDriverRotation.x = 0.f;
	m_HeadPose.qWorldFromDriverRotation.y = 0.f;
	m_HeadPose.qWorldFromDriverRotation.z = 0.f;
	m_HeadPose.qWorldFromDriverRotation.w = 1.f;

	m_HeadPose.vecWorldFromDriverTranslation[0] = 0.f;
	m_HeadPose.vecWorldFromDriverTranslation[1] = 0.f;
	m_HeadPose.vecWorldFromDriverTranslation[2] = 0.f;

	// adjust for the tilt of the rigel in the headset.
	Quaternion values = Quaternion::Euler(m_centerEyeFromDeviceRotationEuler);
	m_HeadPose.qDriverFromHeadRotation.x = values.x;
	m_HeadPose.qDriverFromHeadRotation.y = values.y;
	m_HeadPose.qDriverFromHeadRotation.z = values.z;
	m_HeadPose.qDriverFromHeadRotation.w = values.w;

	// "Driver" and "Head" are confusing names.
	// "Head" as used here in OpenVR is the tracking device.
	// "Driver" is the position from which to render -- i.e., the center eye.
	m_HeadPose.vecDriverFromHeadTranslation[0] = m_centerEyeFromDevicePosition.x;
	m_HeadPose.vecDriverFromHeadTranslation[1] = m_centerEyeFromDevicePosition.y;
	m_HeadPose.vecDriverFromHeadTranslation[2] = m_centerEyeFromDevicePosition.z;

	m_hasEyeData = false;
	m_LastLeftEye = Vector3::Left() * 0.032f;
	m_LastRightEye = Vector3::Right() * 0.032f;
}

CNorthStarDeviceDriver::~CNorthStarDeviceDriver()
{
	s_logger.DriverLog("NorthStar::Deconstructor");
	m_bExiting = true;
}

EVRInitError CNorthStarDeviceDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
	// Note: we may want to move a lot of the work we do in the constructor to
	// activate instead...
	s_logger.DriverLog("NorthStar::Activate\n");

	m_unObjectId = unObjectId;
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

	m_leftEye.LoadOpticalData(s_strLeftEye);
	m_rightEye.LoadOpticalData(s_strRightEye);

	m_LastLeftEye = m_leftEye.GetEyePosition();
	m_LastRightEye = m_rightEye.GetEyePosition();

	s_logger.DriverLog("Logging Left Eye Data\n");
	m_leftEye.Log();

	s_logger.DriverLog("Logging Right Eye Data\n");
	m_rightEye.Log();

	// Icons can be configured in code or automatically configured by an external file "drivername\resources\driver.vrresources".
	// Icon properties NOT configured in code (post Activate) are then auto-configured by the optional presence of a driver's "drivername\resources\driver.vrresources".
	// In this manner a driver can configure their icons in a flexible data driven fashion by using an external file.
	//
	// The structure of the driver.vrresources file allows a driver to specialize their icons based on their HW.
	// Keys matching the value in "Prop_ModelNumber_String" are considered first, since the driver may have model specific icons.
	// An absence of a matching "Prop_ModelNumber_String" then considers the ETrackedDeviceClass ("HMD", "Controller", "GenericTracker", "TrackingReference")
	// since the driver may have specialized icons based on those device class names.
	//
	// An absence of either then falls back to the "system.vrresources" where generic device class icons are then supplied.
	//
	// Please refer to "bin\drivers\sample\resources\driver.vrresources" which contains this sample configuration.
	//
	// "Alias" is a reserved key and specifies chaining to another json block.
	//
	// In this sample configuration file (overly complex FOR EXAMPLE PURPOSES ONLY)....
	//
	// "Model-v2.0" chains through the alias to "Model-v1.0" which chains through the alias to "Model-v Defaults".
	//
	// Keys NOT found in "Model-v2.0" would then chase through the "Alias" to be resolved in "Model-v1.0" and either resolve their or continue through the alias.
	// Thus "Prop_NamedIconPathDeviceAlertLow_String" in each model's block represent a specialization specific for that "model".
	// Keys in "Model-v Defaults" are an example of mapping to the same states, and here all map to "Prop_NamedIconPathDeviceOff_String".
	//
	bool bSetupIconUsingExternalResourceFile = true;
	if (!bSetupIconUsingExternalResourceFile)
	{
		// Setup properties directly in code.
		// Path values are of the form {drivername}\icons\some_icon_filename.png
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{sample}/icons/headset_sample_status_off.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{sample}/icons/headset_sample_status_searching.gif");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{sample}/icons/headset_sample_status_searching_alert.gif");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{sample}/icons/headset_sample_status_ready.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{sample}/icons/headset_sample_status_ready_alert.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{sample}/icons/headset_sample_status_error.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{sample}/icons/headset_sample_status_standby.png");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{sample}/icons/headset_sample_status_ready_low.png");
	}

	DriverPose_t hand_pose = { 0.f };
	hand_pose.deviceIsConnected = true;
	hand_pose.poseIsValid = true;
	hand_pose.willDriftInYaw = false;
	hand_pose.shouldApplyHeadModel = false;
	hand_pose.poseTimeOffset = 0.f;
	hand_pose.result = ETrackingResult::TrackingResult_Running_OK;
	hand_pose.qDriverFromHeadRotation = { 1.f,0.f,0.f,0.f };
	hand_pose.qWorldFromDriverRotation = { 1.f,0.f,0.f,0.f };

	hand_pose.vecPosition[0] = 0.f;
	hand_pose.vecPosition[1] = 1.6f;  // try to start them off the floor roughly hand-ish height.
	hand_pose.vecPosition[2] = 0.f;

	VRControllerState_t hand_state;
	hand_state.ulButtonPressed = hand_state.ulButtonTouched = 0;

	return VRInitError_None;
}

void CNorthStarDeviceDriver::Deactivate()
{
	s_logger.DriverLog("NorthStar::Deactivate Headset\n");
	m_bExiting = true;
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;

}

void CNorthStarDeviceDriver::EnterStandby()
{
	s_logger.DriverLog("NorthStar::Entering StandBy mode\n");
}

void* CNorthStarDeviceDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
	{
		return (vr::IVRDisplayComponent*)this;
	}
	return NULL;
}

void CNorthStarDeviceDriver::PowerOff()
{
	s_logger.DriverLog("NorthStar::Headset power off\n");
}

/** debug request from a client */
void CNorthStarDeviceDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

void CNorthStarDeviceDriver::GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight)
{
	*pnX = m_nWindowX;
	*pnY = m_nWindowY;
	*pnWidth = m_nWindowWidth;
	*pnHeight = m_nWindowHeight;
}

bool CNorthStarDeviceDriver::IsDisplayOnDesktop()
{
	return true;
}

bool CNorthStarDeviceDriver::IsDisplayRealDisplay()
{
	return false;
}

void CNorthStarDeviceDriver::GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight)
{
	*pnWidth = m_nRenderWidth;
	*pnHeight = m_nRenderHeight;
}

void CNorthStarDeviceDriver::GetEyeOutputViewport(EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight)
{
	*pnY = 0;
	*pnWidth = m_nWindowWidth / 2;
	*pnHeight = m_nWindowHeight;

	if (eEye == Eye_Left)
	{
		*pnX = 0;
	}
	else {
		*pnX = m_nWindowWidth / 2;
	}
}


void CNorthStarDeviceDriver::GetProjectionRaw(EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom)
{

	if (eEye == Eye_Left) {
		Vector4 eyeproj = m_leftEye.GetCameraProjection();
		*pfLeft = eyeproj.x;
		*pfRight = eyeproj.y;
		*pfTop = eyeproj.z;
		*pfBottom = eyeproj.w;
	}
	else {
		Vector4 eyeproj = m_rightEye.GetCameraProjection();
		*pfLeft = eyeproj.x;
		*pfRight = eyeproj.y;
		*pfTop = eyeproj.z;
		*pfBottom = eyeproj.w;
	}
}

DistortionCoordinates_t CNorthStarDeviceDriver::ComputeDistortion(EVREye eEye, float fU, float fV)
{
	m_uvReadCount++;
	Vector2 Res;
	Vector2 UV;
	UV.x = fU;
	UV.y = 1.f - fV;

	DistortionCoordinates_t coordinates;

	if (eEye == Eye_Left) {
		Res = m_leftEye.DisplayUVToRenderUVPreviousSeed(UV);
		//Res = m_leftEye.SolveDisplayUVToRenderUV(UV, Vector2(0.5f, 0.5f), 25);
		//s_logger.DriverLog("Left Eye UV %g %g %g %g\n", fU, fV, Res.x, Res.y);
	}
	else {

		Res = m_rightEye.DisplayUVToRenderUVPreviousSeed(UV);
		//Res = m_rightEye.SolveDisplayUVToRenderUV(UV, Vector2(0.5f, 0.5f), 25);
		//s_logger.DriverLog("Right Eye UV %g %g %g %g\n", fU, fV, Res.x, Res.y);
	}
	coordinates.rfBlue[0] = Res.x;
	coordinates.rfBlue[1] = 1.f - Res.y;
	coordinates.rfGreen[0] = Res.x;
	coordinates.rfGreen[1] = 1.f - Res.y;
	coordinates.rfRed[0] = Res.x;
	coordinates.rfRed[1] = 1.f - Res.y;

	return coordinates;
}


DriverPose_t CNorthStarDeviceDriver::GetPose()
{
	Vector3 devicePosition;
	Quaternion deviceRotation;

	//Set the head pose to just above the origin
	m_HeadPose.poseIsValid = true;
	m_HeadPose.result = ETrackingResult::TrackingResult_Running_OK;

	// Get device pose.
	devicePosition = Vector3(0.0f, 0.0f, 0.0f);
	deviceRotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	Pose centerEyeFromDevicePose = Pose(
		m_centerEyeFromDevicePosition,
		Quaternion::Euler(m_centerEyeFromDeviceRotationEuler)
	);
	Pose deviceFromCenterEyePose = centerEyeFromDevicePose.Inverse();

	Pose devicePose = Pose(devicePosition, deviceRotation);

	// Get center eye pose.
	Pose centerEyePose = devicePose * centerEyeFromDevicePose;

	// If we have eye tracking data, apply additional offsets in center-eye
	// space. Otherwise, the "tracked" center eye pose will simply be
	// the headset-origin center eye pose.
	Pose trackedCenterEyePose = centerEyePose;
	Vector3 baseCenterEye = Vector3::Zero(); // Origin, in center-eye space.
	Vector3 trackedCenterEye;
	Vector3 trackedFromBasePosition;

	Vector3 baseEyeDiff = Vector3::Right();
	Vector3 trackedEyeDiff;


	trackedCenterEye = (m_leftEye.eyePosition + m_rightEye.eyePosition) * 0.5f;
	trackedCenterEye.z = -trackedCenterEye.z;
	trackedFromBasePosition = trackedCenterEye - baseCenterEye;
	trackedEyeDiff = (m_rightEye.eyePosition - m_leftEye.eyePosition).Normalized();

	Matrix4x4 trackedFromBaseRotation =
		Matrix4x4::RotationAlign(baseEyeDiff, trackedEyeDiff);

	// Alternative formulation of trackedFromBaseRotation, directly to
	// a Quaternion. Gives the same result, tested 8/24 (-Nick).
	Quaternion trackedFromBaseRotation2 =
		Quaternion::FromToRotation(baseEyeDiff, trackedEyeDiff);

	Pose trackedFromBasePose = Pose(
		trackedFromBasePosition,
		Quaternion::FromMatrix(trackedFromBaseRotation)
	);
	trackedCenterEyePose = centerEyePose * trackedFromBasePose;

	// Update the ClipToWorld in the calibration.
	baseEyeDiff = Vector3(baseEyeDiff.x, -baseEyeDiff.y, baseEyeDiff.z);
	trackedEyeDiff = Vector3(trackedEyeDiff.x, -trackedEyeDiff.y, trackedEyeDiff.z);
	Matrix4x4 trackedFromBaseRotation_UnityConvention = Matrix4x4::RotationAlign(baseEyeDiff, trackedEyeDiff);
	trackedFromBaseRotation_UnityConvention = trackedFromBaseRotation_UnityConvention.Inverse();
	// Update the calibration eyes' clipToWorld matrix to account for the
	// fact that the eye-diff rotation rotates the OpenVR cameras as well.
	m_leftEye.UpdateClipToWorld(trackedFromBaseRotation_UnityConvention);
	m_rightEye.UpdateClipToWorld(trackedFromBaseRotation_UnityConvention);

	Pose finalDevicePose = trackedCenterEyePose * deviceFromCenterEyePose;

	Vector3 finalDevicePosition = finalDevicePose.position;
	m_HeadPose.vecPosition[0] = finalDevicePosition.x;
	m_HeadPose.vecPosition[1] = finalDevicePosition.y;
	m_HeadPose.vecPosition[2] = finalDevicePosition.z;

	Quaternion finalDeviceRotation = finalDevicePose.rotation;
	m_HeadPose.qRotation.x = finalDeviceRotation.x;
	m_HeadPose.qRotation.y = finalDeviceRotation.y;
	m_HeadPose.qRotation.z = finalDeviceRotation.z;
	m_HeadPose.qRotation.w = finalDeviceRotation.w;

	return m_HeadPose;
}


void CNorthStarDeviceDriver::RunFrame()
{
	// since hands are relative to head position we need
	// the head postion to put the hands in world space.
	DriverPose_t HeadsetPose = GetPose();


	// Apply head pose change even when m_enableHeadTracking is false
	// This forces SteamVR to render the image
	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, HeadsetPose, sizeof(DriverPose_t));
}



//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
//=======================================================================================================================================
EVRInitError CNorthStarDriver::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	s_logger.SetLogger(vr::VRDriverLog());


	m_pNullHmdLatest = new CNorthStarDeviceDriver();
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pNullHmdLatest->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pNullHmdLatest);
	return VRInitError_None;
}

void CNorthStarDriver::Cleanup()
{
	delete m_pNullHmdLatest;
	m_pNullHmdLatest = NULL;
}

void CNorthStarDriver::RunFrame()
{

	if (m_pNullHmdLatest) {
		m_pNullHmdLatest->RunFrame();
	}
}


CNorthStarDriver::~CNorthStarDriver() {
	delete m_pNullHmdLatest;
	m_pNullHmdLatest = NULL;
}
