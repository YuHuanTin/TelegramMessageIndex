//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H

#include "Config/ProgramConfig.h"
#include "Core/TelegramClientCore.h"

class TelegramClientCore;

class Functions {
    std::function<void(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message>)> message_parser_;

    std::shared_ptr<ProgramConfig> configServicePtr;

    TelegramClientCore *core_;
public:
    explicit Functions(TelegramClientCore *Core, std::shared_ptr<ProgramConfig> ProgramConfig_);

    void func_history();

    void func_parse_update_message(td::tl::unique_ptr<td::td_api::message> Message);

    void func_spy_picture_by_id();
};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
