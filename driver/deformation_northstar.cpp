// Copyright (c) 2018 Leap Motion. All rights reserved. Proprietary and confidential.
#include "deformation_northstar.h"
#include "driverlog_northstar.h"

OpticalSystem::OpticalSystem(const OpticalSystem& _in) {
    ellipseMinorAxis = _in.ellipseMinorAxis;
    ellipseMajorAxis = _in.ellipseMajorAxis;
    screenForward = _in.screenForward;
    screenPosition = _in.screenPosition;
    eyePosition = _in.eyePosition;
    worldToSphereSpace = _in.worldToSphereSpace;
    sphereToWorldSpace = _in.sphereToWorldSpace;
    worldToScreenSpace = _in.worldToScreenSpace;
    clipToWorld = _in.clipToWorld;
    cameraProjection = _in.cameraProjection;
};

OpticalSystem::OpticalSystem(
  float  a1, float  a2, float  a3, float  a4, float  a5, float  a6, float  a7, float  a8, float  a9, float a10,
    float a11, float a12, float a13, float a14, float a15, float a16, float a17, float a18, float a19, float a20,
    float a21, float a22, float a23, float a24, float a25, float a26, float a27, float a28, float a29, float a30,
    float a31, float a32, float a33, float a34, float a35, float a36, float a37, float a38, float a39, float a40,
    float a41, float a42, float a43, float a44, float a45, float a46, float a47, float a48, float a49, float a50,
    float a51, float a52, float a53, float a54, float a55, float a56, float a57, float a58, float a59, float a60,
    float a61, float a62, float a63, float a64, float a65, float a66, float a67, float a68, float a69, float a70,
    float a71, float a72, float a73, float a74, float a75, float a76, float a77, float a78, float a79
) {

    ellipseMinorAxis = a1;
    ellipseMajorAxis = a2;
    screenForward.x = a3; screenForward.y = a4; screenForward.z = a5;
    screenPosition.x = a6; screenPosition.y = a7;  screenPosition.z = a8;
    eyePosition.x = a9; eyePosition.y = a10; eyePosition.z = a11;
    worldToSphereSpace.m00 = a12; worldToSphereSpace.m01 = a13; worldToSphereSpace.m02 = a14; worldToSphereSpace.m03 = a15;
    worldToSphereSpace.m10 = a16; worldToSphereSpace.m11 = a17; worldToSphereSpace.m12 = a18; worldToSphereSpace.m13 = a19;
    worldToSphereSpace.m20 = a20; worldToSphereSpace.m21 = a21; worldToSphereSpace.m22 = a22; worldToSphereSpace.m23 = a23;
    worldToSphereSpace.m30 = a24; worldToSphereSpace.m31 = a25; worldToSphereSpace.m32 = a26; worldToSphereSpace.m33 = a27;

    sphereToWorldSpace.m00 = a28; sphereToWorldSpace.m01 = a29; sphereToWorldSpace.m02 = a30; sphereToWorldSpace.m03 = a31;
    sphereToWorldSpace.m10 = a32; sphereToWorldSpace.m11 = a33; sphereToWorldSpace.m12 = a34; sphereToWorldSpace.m13 = a35;
    sphereToWorldSpace.m20 = a36; sphereToWorldSpace.m21 = a37; sphereToWorldSpace.m22 = a38; sphereToWorldSpace.m23 = a39;
    sphereToWorldSpace.m30 = a40; sphereToWorldSpace.m31 = a41; sphereToWorldSpace.m32 = a42; sphereToWorldSpace.m33 = a43;

    worldToScreenSpace.m00 = a44; worldToScreenSpace.m01 = a45; worldToScreenSpace.m02 = a46; worldToScreenSpace.m03 = a47;
    worldToScreenSpace.m10 = a48; worldToScreenSpace.m11 = a49; worldToScreenSpace.m12 = a50; worldToScreenSpace.m13 = a51;
    worldToScreenSpace.m20 = a52; worldToScreenSpace.m21 = a53; worldToScreenSpace.m22 = a54; worldToScreenSpace.m23 = a55;
    worldToScreenSpace.m30 = a56; worldToScreenSpace.m31 = a57; worldToScreenSpace.m32 = a58; worldToScreenSpace.m33 = a59;

    clipToWorld.m00 = a60; clipToWorld.m01 = a61; clipToWorld.m02 = a62; clipToWorld.m03 = a63;
    clipToWorld.m10 = a64; clipToWorld.m11 = a65; clipToWorld.m12 = a66; clipToWorld.m13 = a67;
    clipToWorld.m20 = a68; clipToWorld.m21 = a69; clipToWorld.m22 = a70; clipToWorld.m23 = a71;
    clipToWorld.m30 = a72; clipToWorld.m31 = a73; clipToWorld.m32 = a74; clipToWorld.m33 = a75;

    cameraProjection.x = a76;
    cameraProjection.y = a77;
    cameraProjection.z = a78;
    cameraProjection.w = a79;
};

void OpticalSystem::Log(char* name, Matrix4x4 _in) {
    m_logger->DriverLog("%s : \n", name);
    m_logger->DriverLog("%f %f %f %f\n", _in.m00, _in.m01, _in.m02, _in.m03);
    m_logger->DriverLog("%f %f %f %f\n", _in.m10, _in.m11, _in.m12, _in.m13);
    m_logger->DriverLog("%f %f %f %f\n", _in.m20, _in.m21, _in.m22, _in.m23);
    m_logger->DriverLog("%f %f %f %f\n", _in.m30, _in.m31, _in.m32, _in.m33);
}

void OpticalSystem::Log() {
    m_logger->DriverLog("ellipseMinorAxis %f\n", ellipseMinorAxis);
    m_logger->DriverLog("ellipseMajorAxis %f\n", ellipseMajorAxis);
    m_logger->DriverLog("screenForward %f %f %f\n", screenForward.x, screenForward.y, screenForward.z);
    m_logger->DriverLog("screenPosition %f %f %f\n", screenPosition.x, screenPosition.y, screenPosition.z);
    m_logger->DriverLog("eyePosition %f %f %f\n", eyePosition.x, eyePosition.y, eyePosition.z);
    m_logger->DriverLog("cameraProjection %f %f %f %f\n", cameraProjection.x, cameraProjection.y, cameraProjection.z, cameraProjection.w);
    Log("worldToSphereSpace", worldToSphereSpace);
    Log("sphereToWorldSpace", sphereToWorldSpace);
    Log("worldToScreenSpace", worldToScreenSpace);
    Log("clipToWorld", clipToWorld);
}

void OpticalSystem::LoadOpticalData(const char* CurrentEye) {

    ellipseMinorAxis = vr::VRSettings()->GetFloat(CurrentEye, "ellipseMinorAxis");
    ellipseMajorAxis = vr::VRSettings()->GetFloat(CurrentEye, "ellipseMajorAxis");

    screenForward.x = vr::VRSettings()->GetFloat(CurrentEye, "screenForward_x");
    screenForward.y = vr::VRSettings()->GetFloat(CurrentEye, "screenForward_y");
    screenForward.z = vr::VRSettings()->GetFloat(CurrentEye, "screenForward_z");

    screenPosition.x = vr::VRSettings()->GetFloat(CurrentEye, "screenPosition_x");
    screenPosition.y = vr::VRSettings()->GetFloat(CurrentEye, "screenPosition_y");
    screenPosition.z = vr::VRSettings()->GetFloat(CurrentEye, "screenPosition_z");

    eyePosition.x = vr::VRSettings()->GetFloat(CurrentEye, "eyePosition_x");
    eyePosition.y = vr::VRSettings()->GetFloat(CurrentEye, "eyePosition_y");
    eyePosition.z = vr::VRSettings()->GetFloat(CurrentEye, "eyePosition_z");

    sphereToWorldSpace.m00 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e00");
    sphereToWorldSpace.m01 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e01");
    sphereToWorldSpace.m02 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e02");
    sphereToWorldSpace.m03 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e03");
    sphereToWorldSpace.m10 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e10");
    sphereToWorldSpace.m11 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e11");
    sphereToWorldSpace.m12 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e12");
    sphereToWorldSpace.m13 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e13");
    sphereToWorldSpace.m20 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e20");
    sphereToWorldSpace.m21 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e21");
    sphereToWorldSpace.m22 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e22");
    sphereToWorldSpace.m23 = vr::VRSettings()->GetFloat(CurrentEye, "sphereToWorldSpace_e23");
    sphereToWorldSpace.m30 = 0.0f;
    sphereToWorldSpace.m31 = 0.0f;
    sphereToWorldSpace.m32 = 0.0f;
    sphereToWorldSpace.m33 = 1.0f;

    worldToScreenSpace.m00 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e00");
    worldToScreenSpace.m01 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e01");
    worldToScreenSpace.m02 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e02");
    worldToScreenSpace.m03 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e03");
    worldToScreenSpace.m10 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e10");
    worldToScreenSpace.m11 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e11");
    worldToScreenSpace.m12 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e12");
    worldToScreenSpace.m13 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e13");
    worldToScreenSpace.m20 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e20");
    worldToScreenSpace.m21 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e21");
    worldToScreenSpace.m22 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e22");
    worldToScreenSpace.m23 = vr::VRSettings()->GetFloat(CurrentEye, "worldToScreenSpace_e23");
    worldToScreenSpace.m30 = 0.0f;
    worldToScreenSpace.m31 = 0.0f;
    worldToScreenSpace.m32 = 0.0f;
    worldToScreenSpace.m33 = 1.0f;

    cameraProjection.x = vr::VRSettings()->GetFloat(CurrentEye, "cameraProjection_x");
    cameraProjection.y = vr::VRSettings()->GetFloat(CurrentEye, "cameraProjection_y");
    cameraProjection.z = vr::VRSettings()->GetFloat(CurrentEye, "cameraProjection_z");
    cameraProjection.w = vr::VRSettings()->GetFloat(CurrentEye, "cameraProjection_w");

    worldToSphereSpace = sphereToWorldSpace.Inverse();

  UpdateClipToWorld(Matrix4x4::Identity());
};

void OpticalSystem::RegenerateMesh() {

  //m_logger->DriverLog("Regenerate mesh called.");

    std::map<float, std::map<float, Vector2> >::iterator outerIter;
    std::map<float, Vector2>::iterator  innerIter;

    // walk all the requested stored UVs and regeneate the response values
    // you probably should have updated the eye postion before doing this :-D
    for (outerIter = m_requestedUVs.begin(); outerIter != m_requestedUVs.end(); outerIter++) {
        for (innerIter = outerIter->second.begin(); innerIter != outerIter->second.end(); innerIter++) {
            Vector2 result = SolveDisplayUVToRenderUV(Vector2(outerIter->first, innerIter->first),
                Vector2(innerIter->second.x, innerIter->second.y), m_iniSolverIters);
            innerIter->second.x = result.x;
            innerIter->second.y = result.y;
        }
    }
}


Vector2 OpticalSystem::RenderUVToDisplayUV(Vector2 inputUV) {
    Vector3 rayDir;
    ViewportPointToRayDirection(inputUV, eyePosition, clipToWorld, rayDir);
    Vector2 curDisplayUV = RenderUVToDisplayUV(rayDir);
    return curDisplayUV;
}

Vector2 OpticalSystem::RenderUVToDisplayUV(Vector3 inputUV) {

    Vector3 sphereSpaceRayOrigin = worldToSphereSpace.MultiplyPoint(eyePosition);
    Vector3 sphereSpaceRayDirection = (worldToSphereSpace.MultiplyPoint(eyePosition + inputUV) - sphereSpaceRayOrigin);
    sphereSpaceRayDirection = sphereSpaceRayDirection / sphereSpaceRayDirection.Magnitude();

    float intersectionTime = intersectLineSphere(sphereSpaceRayOrigin, sphereSpaceRayDirection, Vector3::Zero(), 0.5f * 0.5f, false);

    if (intersectionTime < 0.f) {
    //m_logger->DriverLog("No line->ellipsoid intersection. %g %g", inputUV.x, inputUV.y);
        return Vector2::zero();
    }
    Vector3 sphereSpaceIntersection = sphereSpaceRayOrigin + (sphereSpaceRayDirection * intersectionTime);

    //Ellipsoid  Normals
    Vector3 sphereSpaceNormal = (Vector3::Zero() - sphereSpaceIntersection) / sphereSpaceIntersection.Magnitude();
    sphereSpaceNormal.x = sphereSpaceNormal.x / powf(ellipseMinorAxis / 2.f, 2.f);
    sphereSpaceNormal.y = sphereSpaceNormal.y / powf(ellipseMinorAxis / 2.f, 2.f);
    sphereSpaceNormal.z = sphereSpaceNormal.z / powf(ellipseMajorAxis / 2.f, 2.f);
    sphereSpaceNormal = sphereSpaceNormal / sphereSpaceNormal.Magnitude();

    Vector3 worldSpaceIntersection = sphereToWorldSpace.MultiplyPoint(sphereSpaceIntersection);
    Vector3 worldSpaceNormal = sphereToWorldSpace.MultiplyVector(sphereSpaceNormal);
    worldSpaceNormal = worldSpaceNormal / worldSpaceNormal.Magnitude();

    Ray firstBounce(worldSpaceIntersection, Vector3::Reflect(inputUV, worldSpaceNormal));
    intersectionTime = intersectPlane(screenForward, screenPosition, firstBounce.m_Origin, firstBounce.m_Direction);

    if (intersectionTime < 0.f) {
    //m_logger->DriverLog("No bounce->screen intersection. %g %g", inputUV.x, inputUV.y);
        return Vector2::zero();
    }
    Vector3 planeIntersection = firstBounce.GetPoint(intersectionTime);

    Vector3 ScreenUVZ = worldToScreenSpace.MultiplyPoint3x4(planeIntersection);

    Vector2 ScreenUV;
    ScreenUV.x = ScreenUVZ.x;
    ScreenUV.y = ScreenUVZ.y;

    Vector2 ScreenUV_Real;

    ScreenUV_Real.y = 1.f - (ScreenUV.x + 0.5f);
    ScreenUV_Real.x = 1.f - (ScreenUV.y + 0.5f);

    return ScreenUV_Real;
}


Vector2 OpticalSystem::SolveDisplayUVToRenderUV(Vector2 inputUV, Vector2 initailGuess,  int iterations) {

    float epsilon = 0.0001f;
    Vector2 curCameraUV;
    curCameraUV.x = initailGuess.x;
    curCameraUV.y = initailGuess.y;
    Vector2 curDisplayUV;

    for (int i = 0; i < iterations; i++) {
        Vector3 rayDir;

        // we can do all three calls below to RenderUVToDisplayUV at the same time via SIMD
        // or better yet we can vectorize across all the uvs if we have a list of them
        curDisplayUV = RenderUVToDisplayUV(curCameraUV);
        Vector2 displayUVGradX = (RenderUVToDisplayUV(curCameraUV + (Vector2(1, 0) * epsilon)) - curDisplayUV) / epsilon;
        Vector2 displayUVGradY = (RenderUVToDisplayUV(curCameraUV + (Vector2(0, 1) * epsilon)) - curDisplayUV) / epsilon;

        Vector2 error = curDisplayUV - inputUV;
        Vector2 step = Vector2::zero();

        if (!displayUVGradX.x == 0.f || !displayUVGradX.y == 0.f) {
            step = step + (displayUVGradX * error.x);
        }
        if (!displayUVGradY.x == 0.f || !displayUVGradY.y == 0.f) {
            step = step + (displayUVGradY * error.y);
        }

        curCameraUV.x = curCameraUV.x - (step.x / 7.f);
        curCameraUV.y = curCameraUV.y - (step.y / 7.f);
    }

    return curCameraUV;
}


Vector2 OpticalSystem::DisplayUVToRenderUVPreviousSeed(Vector2 inputUV) {
    // if we don't find a point we generate it and add it to our list
    Vector2 curDisplayUV;

    std::map<float, std::map<float, Vector2> >::iterator outerIter;
    outerIter = m_requestedUVs.find(inputUV.x);
    if (outerIter == m_requestedUVs.end()) {
        // if the outer value is not there we know the inner is not
        // so we just slam both in and call it a day
        std::map<float, Vector2> inner;
        curDisplayUV = SolveDisplayUVToRenderUV(inputUV, Vector2( 0.5f, 0.5f), m_iniSolverIters);

        inner.insert(std::pair<float, Vector2>(inputUV.y, curDisplayUV));
        m_requestedUVs.insert(std::pair<float, std::map<float, Vector2>>(inputUV.x, inner));
        //Logger->DriverLog("NorthStar  Generated UV %g %g ", inputUV.x, inputUV.y);

    }
    else {
        std::map<float, Vector2>::iterator  innerIter;
        innerIter = outerIter->second.find(inputUV.y);

        if (innerIter == outerIter->second.end()) {
            // we assume there is no reason to ask for the same value so no
            // need to check if it exists already so just add it.
            curDisplayUV = SolveDisplayUVToRenderUV(inputUV, Vector2(0.5f, 0.5f), m_iniSolverIters);

            outerIter->second.insert(std::pair<float, Vector2>(inputUV.y, curDisplayUV));
            //Logger->DriverLog("NorthStar  Generated UV %g %g ", outerIter->first, innerIter->first);
        }
        else {
            // return the value we found
            // hopefully we have remashed at least once otherwise
            // we are giving back the same points again
            //curDisplayUV.x = innerIter->second.x;
            //curDisplayUV.y = innerIter->second.y;

            curDisplayUV = SolveDisplayUVToRenderUV(inputUV, Vector2(innerIter->second.x, innerIter->second.y), m_optSolverIters);

            //Logger->DriverLog("NorthStar  Found UV %g %g ", outerIter->first, innerIter->first);
        }
    }
    return curDisplayUV;
}

