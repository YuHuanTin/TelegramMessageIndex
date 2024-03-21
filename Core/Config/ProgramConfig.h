//
// Created by YuHuanTin on 2024/3/19.
//

#ifndef TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
#define TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H


#include "../Register/StringRegister.h"

#include <fstream>
#include <string>
#include <vector>
#include <map>

class ProgramConfig {
private:
    std::fstream file_;

    StringRegister                     mapRegisterdName_;
    std::map<std::string, std::string> mapConfigValue_;

public:


    ProgramConfig();

    /**
     * 从内存 map 中读取配置
     * @param Key 
     * @return 
     */
    std::string read(REGISTER::CONFIG_STRING_NAME Key);

    /**
     * 从内存 map 中读取列表配置
     * @param Key 
     * @return 
     */
    std::vector<std::string> read_lists(REGISTER::CONFIG_STRING_NAME Key);

    /**
     * 将配置写入内存 map
     * @param Key 
     * @param Value 
     * @return 
     */
    bool write(REGISTER::CONFIG_STRING_NAME Key, const std::string &Value);

    /**
     * 将列表配置写入内存 map
     * @param Key 
     * @param Value 
     * @return 
     */
    bool write_lists(REGISTER::CONFIG_STRING_NAME Key, const std::vector<std::string> &Value);

    /**
     * 真正写入配置到文件中去
     * @return 
     */
    bool refresh();

    ~ProgramConfig();
};


#endif //TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
