//
// Created by YuHuanTin on 2024/3/20.
//

#include "ConsoleCtrlCapturer.h"

#include <print>

BOOL ConsoleCtrlCapturer::ControlHandler(const DWORD ctrl_type) {
    switch (ctrl_type) {
        case CTRL_C_EVENT:
            ConsoleCtrlCapturer::is_capture_ctrl_c_ = true;
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            return TRUE;
        default:
            return FALSE;
    }
}

ConsoleCtrlCapturer::ConsoleCtrlCapturer() {
    if (!SetConsoleCtrlHandler(ConsoleCtrlCapturer::ControlHandler, TRUE)) {
        std::println("SetConsoleCtrlHandler failed: {}", GetLastError());
    }
}

bool ConsoleCtrlCapturer::IsCaptureCtrlC() { return is_capture_ctrl_c_; }

ConsoleCtrlCapturer::~ConsoleCtrlCapturer() {
    if (!SetConsoleCtrlHandler(ConsoleCtrlCapturer::ControlHandler, FALSE)) {
        std::println("SetConsoleCtrlHandler failed: {}", GetLastError());
    }

    ConsoleCtrlCapturer::is_capture_ctrl_c_ = false;
}
