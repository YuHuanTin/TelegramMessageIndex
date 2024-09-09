//
// Created by AFETT on 2024/7/30.
//
module;

export module Core.Register.FunctionRegister;

import std;
import std.compat;

export class TelegramClientCore;

export namespace REGISTER {
    enum class FUNCTION_NAME {
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


export struct FunctionRegister {
    std::unordered_map<std::string, REGISTER::FUNCTION_NAME>                                mapFuncStr_;
    std::unordered_map<REGISTER::FUNCTION_NAME, std::function<void(TelegramClientCore *)> > mapFunc_;

    FunctionRegister();

    void displayCommands();
};

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
