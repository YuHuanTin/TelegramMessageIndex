//
// Created by YuHuanTin on 2024/3/21.
//

#include "FunctionRegister.h"

#include <print>

FunctionRegister::FunctionRegister() {
    this->mapFuncStr_["u"]                 = REGISTER::FUNCTION_NAME::FUNCTION_UPDATE;
    this->mapFuncStr_["c"]                 = REGISTER::FUNCTION_NAME::FUNCTION_GET_CHAT_LIST;
    this->mapFuncStr_["m"]                 = REGISTER::FUNCTION_NAME::FUNCTION_SEND_MESSAGE;
    this->mapFuncStr_["me"]                = REGISTER::FUNCTION_NAME::FUNCTION_ME;
    this->mapFuncStr_["l"]                 = REGISTER::FUNCTION_NAME::FUNCTION_LOGOUT;
    this->mapFuncStr_["history"]           = REGISTER::FUNCTION_NAME::FUNCTION_HISTORY;
    this->mapFuncStr_["spy_picture_by_id"] = REGISTER::FUNCTION_NAME::FUNCTION_SPY_PICTURE_BY_ID;

    /**
     * // todo
     * WARN : this->mapFunc_ need registed by function
     */
}

void FunctionRegister::displayCommands() {
    std::println("[q]: quit\n"
                 "[u]: check for updates and request results\n"
                 "[close]: close client\n"
                 "[c]: show chats\n"
                 "[m <chat_id> <text>]: send message\n"
                 "[me]: show self\n"
                 "[l]: logout\n"
                 "[history]: get history message\n"
                 "[spy_picture_by_id]: spy user id send picture");
}
