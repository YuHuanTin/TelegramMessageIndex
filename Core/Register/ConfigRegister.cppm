
module;

#include <cstdint>

export module ConfigRegister;

import StringRegister;
import ConfigManager;
import Logger;

export namespace REGISTER
{
    using ProxySettings = std::tuple<bool, std::string, std::uint32_t>;

    enum class SpyPictureMode : uint8_t {
        WhiteList,
        BlackList
    };


    namespace CONFIGERS
    {
        ConfigManager Config_Log_Level(STRING_POOL::config_log_level, LogFormat::Debug);
        ConfigManager Config_Proxy_Setting(STRING_POOL::config_proxy_setting, ProxySettings { false, "", 0 });
        ConfigManager Config_Last_Login_Phone_Number(STRING_POOL::config_last_login_phone_number);
        ConfigManager Config_Spy_Picture_Mode(STRING_POOL::config_spy_picture_mode, SpyPictureMode::WhiteList);
        ConfigManager Config_Spy_Picture_By_Id_List(STRING_POOL::config_spy_picture_by_id_list);
    }
}
