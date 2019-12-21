// Copyright (c) 2018 Leap Motion. All rights reserved. Proprietary and confidential.

#pragma once

#include <string>
#include <openvr_driver.h>

#include <stdio.h>
#include <stdarg.h>

const int MAXLOGLEN = 2048;

class LPDriverLogger  {
public:

    LPDriverLogger(vr::IVRDriverLog* log) {
        m_OpenVRLogger = log;
    };

    LPDriverLogger() {
        m_OpenVRLogger = nullptr;
    };

    void SetLogger(vr::IVRDriverLog* log) {
        m_OpenVRLogger = log;
    }

    ~LPDriverLogger() {
        m_OpenVRLogger = nullptr;
    };


    void DriverLog(const char *pMsgFormat, ...)
    {
        va_list args;
        va_start(args, pMsgFormat);

        char buf[MAXLOGLEN];
        vsnprintf_s(buf, sizeof(buf), pMsgFormat, args);

        if (m_OpenVRLogger)
            m_OpenVRLogger->Log(buf);

        va_end(args);
    }

  // Slight short-hand for logging a newline.
  void Endl() {
    DriverLog("\n");
  }

protected:
    vr::IVRDriverLog* m_OpenVRLogger;
};

static LPDriverLogger s_logger;