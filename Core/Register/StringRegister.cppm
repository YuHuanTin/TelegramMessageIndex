
module;

export module StringRegister;

export import std;

export namespace REGISTER
{
    namespace STRING_POOL
    {
        constexpr std::string_view normal_config_file_name        = "config";
        constexpr std::string_view config_log_level               = "log_level";
        constexpr std::string_view config_proxy_setting           = "proxy_setting";
        constexpr std::string_view config_last_login_phone_number = "last_login_phone_number";
        constexpr std::string_view config_spy_picture_mode        = "spy_picture_mode";
        constexpr std::string_view config_spy_picture_by_id_list  = "spy_picture_by_id_list";
    };
}
