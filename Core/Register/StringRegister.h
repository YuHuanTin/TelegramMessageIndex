//
// Created by YuHuanTin on 2024/3/21.
//

#ifndef TELEGRAMMESSAGEINDEX_STRINGREGISTER_H
#define TELEGRAMMESSAGEINDEX_STRINGREGISTER_H

#include <unordered_map>
#include <string>


namespace REGISTER {
    enum class CONFIG_STRING_NAME {
        proxy_host,
        proxy_port,
        last_login_phone_number,
        spy_picture_by_id_list
    };
}


struct StringRegister {
    std::unordered_map<REGISTER::CONFIG_STRING_NAME, std::string> mapRegisterdStr_;

    StringRegister();
};


#endif //TELEGRAMMESSAGEINDEX_STRINGREGISTER_H
