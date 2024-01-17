//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H


#include "TelegramClientCore.h"


class Functions {
    friend class TelegramClientCore;

private:

    using Object = td::td_api::object_ptr<td::td_api::Object>;
public:

    static void get_history_messages(TelegramClientCore *Core);

    static void parse_update_message(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::updateNewMessage> &&Message); 


};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
