//
// Created by AFETT on 2024/7/27.
//

#ifndef CONFIGREGISTER_H
#define CONFIGREGISTER_H
#include <string>
#include <utility>

#include "StringRegister.h"
#include "../Config/ProgramConfig.h_"


namespace REGISTER {
    namespace DETAIL {
        class Config {
            inline static ProgramConfig value_reader_;
            std::string_view            key_;
            bool                        has_default_value_ { false };
            std::string                 default_value_;

        public:
            Config() = delete;

            explicit Config(std::string_view Key)
                : key_(Key) {}

            Config(std::string_view Key, bool Has_default_value, std::string Default_value)
                : key_(Key), has_default_value_(Has_default_value), default_value_(std::move(Default_value)) {}

            [[nodiscard]] std::string Value() {
                auto value = value_reader_.Read(key_);
                if (value.empty() && has_default_value_) {
                    return default_value_;
                }
                return value;
            }

            void Set_Value(const std::string &Value) const {
                value_reader_.Write(key_, Value);
            }
        };
    }


    namespace CONFIGERS {
        inline DETAIL::Config Config_Proxy_Host(STRING_POOL::config_proxy_host);
        inline DETAIL::Config Config_Proxy_Port(STRING_POOL::config_proxy_port);
        inline DETAIL::Config Config_Last_Login_Phone_Number(STRING_POOL::config_last_login_phone_number);
        inline DETAIL::Config Config_Spy_Picture_Mode(STRING_POOL::config_spy_picture_mode, true, "white list");
        inline DETAIL::Config Config_Spy_Picture_By_Id_List(STRING_POOL::config_spy_picture_by_id_list);
    }
}


#endif //CONFIGREGISTER_H
