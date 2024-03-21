//
// Created by YuHuanTin on 2024/3/20.
//

#ifndef TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H
#define TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H

#include "windows.h"

class ConsoleCtrlCapturer {
    static bool capture_ctrl_c;

    static BOOL WINAPI control_handler(DWORD ctrl_type);

public:
    ConsoleCtrlCapturer();

    bool is_capture_ctrl_c() { return capture_ctrl_c; }

    ~ConsoleCtrlCapturer();
};


#endif //TELEGRAMMESSAGEINDEX_CONSOLECTRLCAPTURER_H
