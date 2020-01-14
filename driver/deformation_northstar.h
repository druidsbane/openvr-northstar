#pragma once

#include <map>
#include "openvr_driver.h"
#include "driverlog_northstar.h"
#include "utility_northstar.h"
#include <chrono>

using namespace std::chrono;

static char* s_strLeftEye = "leftEye";
static char* s_strRightEye = "rightEye";

class OpticalSystem {
public:
    OpticalSystem() {
     };

    OpticalSystem(const OpticalSystem& _in);

    OpticalSystem(
        float  a1, float  a2, float  a3, float  a4, float  a5, float  a6, float  a7, float  a8, float  a9, float a10,
        float a11, float a12, float a13, float a14, float a15, float a16, float a17, float a18, float a19, float a20,
        float a21, float a22, float a23, float a24, float a25, float a26, float a27, float a28, float a29, float a30,
        float a31, float a32, float a33, float a34, float a35, float a36, float a37, float a38, float a39, float a40,
        float a41, float a42, float a43, float a44, float a45, float a46, float a47, float a48, float a49, float a50,
        float a51, float a52, float a53, float a54, float a55, float a56, float a57, float a58, float a59, float a60,
        float a61, float a62, float a63, float a64, float a65, float a66, float a67, float a68, float a69, float a70,
        float a71, float a72, float a73, float a74, float a75, float a76, float a77, float a78, float a79);

    void Log(char* name, Matrix4x4 _in);

    void Log();

    void LoadOpticalData(const char* CurrentEye);

    Vector3 GetEyePosition() { return eyePosition; }

    Vector2 RenderUVToDisplayUV(Vector3 inputUV);

    Vector2 RenderUVToDisplayUV(Vector2 inputUV);

    Vector2 SolveDisplayUVToRenderUV(Vector2 inputUV, Vector2 initailGuess, int iterations);

    Vector2 DisplayUVToRenderUVPreviousSeed(Vector2 inputUV);

    void RegenerateMesh();

    void UpdateEyePosition(const Vector3 pos) {
        eyePosition.x = pos.x;
        eyePosition.y = pos.y;
        eyePosition.z = pos.z;
    }

    const Vector4 GetCameraProjection() {
        return cameraProjection;
    }

    void SetLogger(LPDriverLogger* newLogger) {
        m_logger = newLogger;
    }


    void setiters(int init, int opt) {
        m_iniSolverIters = init;
        m_optSolverIters = opt;
    }

    void UpdateClipToWorld(Matrix4x4 eyeRotationMatrix) {
    // TEST DELETEME
    float time = clock() / (float)CLOCKS_PER_SEC;
    float osc = sinf(time * 2 * LP_PI);
    Matrix4x4 eyeToWorld = Matrix4x4::Translate(eyePosition) *
      eyeRotationMatrix;
    eyeToWorld.m02 *= -1;
    eyeToWorld.m12 *= -1;
    eyeToWorld.m22 *= -1;
    clipToWorld = eyeToWorld * cameraProjection.ComposeProjection().Inverse();
    }

    Vector3 eyePosition;

  inline void ViewportPointToRayDirection(Vector2 UV, Vector3 cameraPosition, Matrix4x4 clipToWorld, Vector3& out) {
    Vector3 tmp;
    tmp.x = UV.x - 0.5f;
    tmp.y = UV.y - 0.5f;
    tmp.z = 0.f;
    Vector3 dir = clipToWorld.MultiplyPoint(tmp * 2.f) - cameraPosition;

    float mag = dir.Magnitude();
    out = dir / dir.Magnitude();
    return;
  }

private:
    LPDriverLogger* m_logger;

    float ellipseMinorAxis;
    float ellipseMajorAxis;
    Vector3 screenForward;
    Vector3 screenPosition;

    Vector4   cameraProjection;
    Matrix4x4 worldToSphereSpace;
    Matrix4x4 sphereToWorldSpace;
    Matrix4x4 worldToScreenSpace;
    Matrix4x4 clipToWorld;

    int m_iniSolverIters;
    int m_optSolverIters;

    std::map<float , std::map<float, Vector2> > m_requestedUVs;
};

// supporting functions
inline Vector3 Project(Vector3 v1, Vector3 v2) {
    Vector3 v2Norm = (v2 / v2.Magnitude());
    return v2Norm * Vector3::Dot(v1, v2Norm);
}

inline float intersectLineSphere(Vector3 Origin, Vector3 Direction, Vector3 spherePos, float SphereRadiusSqrd, bool frontSide = true) {
    Vector3 L = spherePos - Origin;
    Vector3 offsetFromSphereCenterToRay = Project(L, Direction) - L;
    return (offsetFromSphereCenterToRay.sqrMagnitude() <= SphereRadiusSqrd) ? Vector3::Dot(L, Direction) - (sqrt(SphereRadiusSqrd - offsetFromSphereCenterToRay.sqrMagnitude()) * (frontSide ? 1.f : -1.f)) : -1.f;
}

inline float intersectPlane(Vector3 n, Vector3 p0, Vector3 l0, Vector3 l) {

    float denom = Vector3::Dot((Vector3::Zero() - n), l);
    if (denom > 1.4e-45f) {
        Vector3 p0l0 = p0 - l0;
        float t = Vector3::Dot(p0l0, (Vector3::Zero() - n)) / denom;
        return t;
    }
    return -1.f;
}