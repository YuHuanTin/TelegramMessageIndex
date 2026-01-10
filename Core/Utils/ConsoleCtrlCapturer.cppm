
module;

#include <Windows.h>

export module ConsoleCtrlCapturer;

import Logger;


export class ConsoleCtrlCapturer {
    inline static bool is_capture_ctrl_c_ = false;

    static BOOL WINAPI ControlHandler(DWORD CtrlType) {
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

public:
    ConsoleCtrlCapturer() {
        if (!SetConsoleCtrlHandler(ControlHandler, TRUE)) {
            LogFormat::LogFormatter<LogFormat::Error>("SetConsoleCtrlHandler failed: {}", GetLastError());
        }
    }


    static bool IsCaptureCtrlC() {
        return is_capture_ctrl_c_;
    }

    ~ConsoleCtrlCapturer() {
        if (!SetConsoleCtrlHandler(ControlHandler, FALSE)) {
            LogFormat::LogFormatter<LogFormat::Error>("SetConsoleCtrlHandler failed: {}", GetLastError());
        }
    }
};
