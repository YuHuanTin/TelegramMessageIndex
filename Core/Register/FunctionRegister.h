//
// Created by YuHuanTin on 2024/3/21.
//

#ifndef TELEGRAMMESSAGEINDEX_FUNCTIONREGISTER_H
#define TELEGRAMMESSAGEINDEX_FUNCTIONREGISTER_H


#include <map>
#include <functional>
#include <string>
#include "../TelegramClientCore.h"

namespace REGISTER {
    enum FUNCTION_NAME {
        FUNCTION_UPDATE,
        FUNCTION_CLOSE,
        FUNCTION_ME,
        FUNCTION_LOGOUT,
        FUNCTION_SEND_MESSAGE,
        FUNCTION_GET_CHAT_LIST,

        FUNCTION_HISTORY,
        FUNCTION_SPY_PICTURE_BY_ID
    };
}

class TelegramClientCore;

struct FunctionRegister {
    std::map<std::string, REGISTER::FUNCTION_NAME>                               mapFuncStr_;
    std::map<REGISTER::FUNCTION_NAME, std::function<void(TelegramClientCore *)>> mapFunc_;

    FunctionRegister();

    void displayCommands();
};


#endif //TELEGRAMMESSAGEINDEX_FUNCTIONREGISTER_H
