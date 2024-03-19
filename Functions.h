//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H


#include "Core/TelegramClientCore.h"

class TelegramClientCore;

class Functions {
public:

    static void func_history(TelegramClientCore *Core);

    static void func_parse_update_message(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message);

    static void func_spy_picture_by_id(TelegramClientCore *Core);
};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
