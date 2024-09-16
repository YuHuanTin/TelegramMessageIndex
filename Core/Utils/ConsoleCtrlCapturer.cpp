//
// Created by YuHuanTin on 2024/3/20.
//

#include "ConsoleCtrlCapturer.h"

#include "Logger.hpp"

// BOOL ConsoleCtrlCapturer::ControlHandler(const DWORD ctrl_type) {
//     switch (ctrl_type) {
//         case CTRL_C_EVENT:
//             ConsoleCtrlCapturer::is_capture_ctrl_c_ = true;
//         case CTRL_BREAK_EVENT:
//         case CTRL_CLOSE_EVENT:
//         case CTRL_LOGOFF_EVENT:
//         case CTRL_SHUTDOWN_EVENT:
//             return TRUE;
//         default:
//             return FALSE;
//     }
// }


BOOL ConsoleCtrlCapturer::ControlHandler(const DWORD CtrlType) {
    switch (CtrlType) {
        case CTRL_C_EVENT:
            is_capture_ctrl_c_ = true;
            return TRUE;
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

ConsoleCtrlCapturer::ConsoleCtrlCapturer() {
    if (!SetConsoleCtrlHandler(ControlHandler, TRUE)) {
        LogFormat::LogFormatter<LogFormat::Error>("SetConsoleCtrlHandler failed: {}", GetLastError());
    }
}

bool ConsoleCtrlCapturer::IsCaptureCtrlC() {
    return is_capture_ctrl_c_;
}

ConsoleCtrlCapturer::~ConsoleCtrlCapturer() {
    if (!SetConsoleCtrlHandler(ControlHandler, FALSE)) {
        LogFormat::LogFormatter<LogFormat::Error>("SetConsoleCtrlHandler failed: {}", GetLastError());
    }
}
