//
// Created by YuHuanTin on 2024/3/20.
//

#ifndef TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H
#define TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H

#include "windows.h"

class ConsoleCtrlCapturer {
    inline static bool is_capture_ctrl_c_ { false };

    static BOOL WINAPI ControlHandler(DWORD ctrl_type);

public:
    ConsoleCtrlCapturer();

    static bool IsCaptureCtrlC();

    ~ConsoleCtrlCapturer();
};


#endif //TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H
