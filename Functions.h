//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H


#include "TelegramClientCore.h"

class Functions {
    friend class TelegramClientCore;

public:

    static void get_history_messages(TelegramClientCore *Core);


};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
