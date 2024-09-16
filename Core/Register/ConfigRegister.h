#pragma once

#include "StringRegister.h"
#include "../Config/ConfigManager.h"
#include "../Utils/Logger.hpp"

namespace REGISTER {
    using ProxySettings = std::tuple<bool, std::string, std::uint32_t>;

    enum class SpyPictureMode : uint8_t {
        WhiteList,
        BlackList
    };


    namespace CONFIGERS {
        inline ConfigManager Config_Log_Level(STRING_POOL::config_log_level, LogFormat::Debug);
        inline ConfigManager Config_Proxy_Setting(STRING_POOL::config_proxy_setting, ProxySettings { false, "", 0 });
        inline ConfigManager Config_Last_Login_Phone_Number(STRING_POOL::config_last_login_phone_number);
        inline ConfigManager Config_Spy_Picture_Mode(STRING_POOL::config_spy_picture_mode, SpyPictureMode::WhiteList);
        inline ConfigManager Config_Spy_Picture_By_Id_List(STRING_POOL::config_spy_picture_by_id_list);
    }
}
