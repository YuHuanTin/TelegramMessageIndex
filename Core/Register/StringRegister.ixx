//
// Created by AFETT on 2024/7/30.
//

export module Core.Register.StringRegister;

import std;

export namespace REGISTER {
    namespace STRING_POOL {
        constexpr std::string_view normal_locale                  = "zh_CN.UTF-8";
        constexpr std::string_view normal_config_file_name        = "config";
        constexpr std::string_view config_proxy_host              = "proxy_host";
        constexpr std::string_view config_proxy_port              = "proxy_port";
        constexpr std::string_view config_last_login_phone_number = "last_login_phone_number";
        constexpr std::string_view config_spy_picture_mode        = "spy_picture_mode";
        constexpr std::string_view config_spy_picture_by_id_list  = "spy_picture_by_id_list";
        constexpr std::string_view info_show_load_proxy_host_port = "load config proxy host: {}\nload config proxy port: {}";
        constexpr std::string_view warn_invalid_config_line       = "invalid config line: {}";
        constexpr std::string_view error_cannot_create_file       = "cannot create file: {}";
        constexpr std::string_view error_cannot_open_file         = "cannot open file: {}";
    };
}
