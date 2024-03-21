//
// Created by YuHuanTin on 2024/3/21.
//

#include "StringRegister.h"

StringRegister::StringRegister() {
    this->mapRegisterdStr_[REGISTER::CONFIG_STRING_NAME::proxy_host]              = "proxy_host";
    this->mapRegisterdStr_[REGISTER::CONFIG_STRING_NAME::proxy_port]              = "proxy_port";
    this->mapRegisterdStr_[REGISTER::CONFIG_STRING_NAME::last_login_phone_number] = "last_login_phone_number";
    this->mapRegisterdStr_[REGISTER::CONFIG_STRING_NAME::spy_picture_by_id_list]  = "spy_picture_by_id_list";


}
