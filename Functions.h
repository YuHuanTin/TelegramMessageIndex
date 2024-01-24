//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H


#include "TelegramClientCore.h"

class TelegramClientCore;

class Functions {
public:

    static void func_history(TelegramClientCore *Core);

    static void parse_update_message(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message);

};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
