//
// Created by YuHuanTin on 2024/3/19.
//

#ifndef TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
#define TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H


#include <fstream>
#include <string>
#include <vector>
#include <map>

class ProgramConfig {
public:
    enum NAME {
        proxy_host,
        proxy_port,
        last_login_phone_number
    };
private:
    std::fstream file;

    std::map<NAME, std::string>        mapRegisterdName;
    std::map<std::string, std::string> mapConfigValue;

public:


    ProgramConfig();

    /**
     * 从内存 map 中读取配置
     * @param Key 
     * @return 
     */
    std::string read(NAME Key);

    /**
     * 将配置写入内存 map
     * @param Key 
     * @param Value 
     * @return 
     */
    bool write(NAME Key, const std::string &Value);

    /**
     * 真正写入配置到文件中去
     * @return 
     */
    bool refresh();

    ~ProgramConfig();
};


#endif //TELEGRAMMESSAGEINDEX_PROGRAMCONFIG_H
