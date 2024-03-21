//
// Created by YuHuanTin on 2024/3/21.
//

#ifndef TELEGRAMMESSAGEINDEX_COREWARPPER_H
#define TELEGRAMMESSAGEINDEX_COREWARPPER_H

#include "TelegramClientCore.h"
#include "Functions/Functions.h"


class CoreWarpper {
    std::unique_ptr<TelegramClientCore> telegramClientCore_;

    static std::unique_ptr<FunctionRegister> register_functions(std::shared_ptr<ProgramConfig> ConfigService);

public:
    explicit CoreWarpper(std::shared_ptr<ProgramConfig> ConfigService);

    CoreWarpper(std::shared_ptr<ProgramConfig> ConfigService, int LogLevel);

    void set_socks5_proxy(const std::string &Proxy_Host, int32_t Proxy_Port);

    void loop();
};


#endif //TELEGRAMMESSAGEINDEX_COREWARPPER_H
