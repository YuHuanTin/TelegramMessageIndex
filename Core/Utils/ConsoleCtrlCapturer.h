//
// Created by YuHuanTin on 2024/3/20.
//

#pragma once


class ConsoleCtrlCapturer {
    inline static bool is_capture_ctrl_c_ = false;

    static BOOL WINAPI ControlHandler(DWORD CtrlType);

public:
    ConsoleCtrlCapturer();

    static bool IsCaptureCtrlC();

    ~ConsoleCtrlCapturer();
};
