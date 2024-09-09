//
// Created by YuHuanTin on 2024/3/21.
//

#ifndef TELEGRAMMESSAGEINDEX_STRINGREGISTER_H
#define TELEGRAMMESSAGEINDEX_STRINGREGISTER_H

#include <string_view>

namespace REGISTER {
    namespace STRING_POOL {
        constexpr static std::string_view normal_locale                  = "zh_CN.UTF-8";
        constexpr static std::string_view normal_config_file_name        = "config";
        constexpr static std::string_view config_proxy_host              = "proxy_host";
        constexpr static std::string_view config_proxy_port              = "proxy_port";
        constexpr static std::string_view config_last_login_phone_number = "last_login_phone_number";
        constexpr static std::string_view config_spy_picture_mode        = "spy_picture_mode";
        constexpr static std::string_view config_spy_picture_by_id_list  = "spy_picture_by_id_list";
        constexpr static std::string_view info_show_load_proxy_host_port = "load config proxy host: {}\nload config proxy port: {}";
        constexpr static std::string_view warn_invalid_config_line       = "invalid config line: {}";
        constexpr static std::string_view error_cannot_create_file       = "cannot create file: {}";
        constexpr static std::string_view error_cannot_open_file         = "cannot open file: {}";
    };
}

#endif //TELEGRAMMESSAGEINDEX_STRINGREGISTER_H
