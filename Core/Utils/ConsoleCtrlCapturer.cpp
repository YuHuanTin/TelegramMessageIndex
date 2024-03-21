//
// Created by YuHuanTin on 2024/3/20.
//

#include "ConsoleCtrlCapturer.h"

#include "windows.h"
#include <print>

bool ConsoleCtrlCapturer::capture_ctrl_c = false;

BOOL ConsoleCtrlCapturer::control_handler(DWORD ctrl_type) {
    switch (ctrl_type) {
        case CTRL_C_EVENT:
            ConsoleCtrlCapturer::capture_ctrl_c = true;
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
    ConsoleCtrlCapturer::capture_ctrl_c = false;

    if (!SetConsoleCtrlHandler(ConsoleCtrlCapturer::control_handler, TRUE)) {
        std::println("SetConsoleCtrlHandler failed: {}", GetLastError());
    }
}

ConsoleCtrlCapturer::~ConsoleCtrlCapturer() {
    if (!SetConsoleCtrlHandler(ConsoleCtrlCapturer::control_handler, FALSE)) {
        std::println("SetConsoleCtrlHandler failed: {}", GetLastError());
    }

    ConsoleCtrlCapturer::capture_ctrl_c = false;
}
