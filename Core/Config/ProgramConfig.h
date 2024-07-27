//
// Created by YuHuanTin on 2024/3/19.
//

#ifndef TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
#define TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H


#include <map>
#include <string>
#include <vector>

#include "../Register/StringRegister.h"

class ProgramConfig {
    std::string                             file_name_ { REGISTER::STRING_POOL::normal_config_file_name };
    std::map<std::string_view, std::string> mapConfigValue_;

public:
    ProgramConfig();

    /**
     * 从内存 map 中读取配置
     * @param Key 
     * @return 
     */
    std::string Read(std::string_view Key);

    /**
     * 从内存 map 中读取列表配置
     * @param Key 
     * @return 
     */
    std::vector<std::string> Read_lists(std::string_view Key);

    /**
     * 将配置写入内存 map
     * @param Key 
     * @param Value 
     * @return 
     */
    bool Write(std::string_view Key, const std::string &Value);

    /**
     * 将列表配置写入内存 map
     * @param Key 
     * @param Value 
     * @return 
     */
    bool Write_lists(std::string_view Key, const std::vector<std::string> &Value);

    /**
     * 真正写入配置到文件中去
     * @return 
     */
    void Refresh();

    ~ProgramConfig();
};


#endif //TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
