//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONS_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONS_H

#include "../Config/ProgramConfig.h"
#include "../TelegramClientCore.h"



class Functions {
    std::shared_ptr<ProgramConfig> configServicePtr;

    // not owned just for use
    TelegramClientCore *core_;
public:
    explicit Functions(TelegramClientCore *Core, std::shared_ptr<ProgramConfig> ConfigService);

    void func_history();

    void func_parse_update_message(td::tl::unique_ptr<td::td_api::message> Message);

    void func_spy_picture_by_id();
};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONS_H
