//
// Created by YuHuanTin on 2024/3/19.
//

#include "ProgramConfig.h"

#include <print>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

ProgramConfig::ProgramConfig() {
    // check if config file exists
    this->file.open("config");

    if (!this->file.is_open()) {
        this->file.clear();
        this->file.open("config", std::ios::out); // 创建文件
        if (!this->file.is_open()) {
            std::println("Can not create config file");
            return;
        }
        this->file.close();
        this->file.open("config", std::ios::in | std::ios::out | std::ios::binary);
        if (!this->file.is_open()) {
            std::println("Can not open config file");
            return;
        }
    }

    // register config
    this->mapRegisterdName[proxy_host]              = "proxy_host";
    this->mapRegisterdName[proxy_port]              = "proxy_port";
    this->mapRegisterdName[last_login_phone_number] = "last_login_phone_number";
    this->mapRegisterdName[spy_picture_by_id_list] = "spy_picture_by_id_list";

    // read config
    std::string                                       line;
    std::vector<std::tuple<std::string, std::string>> vecConfigKV;
    while (std::getline(this->file, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            vecConfigKV.emplace_back(line.substr(0, pos), line.substr(pos + 1));
        }
    }

    std::for_each(vecConfigKV.begin(), vecConfigKV.end(), [this](auto &item) {
        this->mapConfigValue[std::get<0>(item)] = std::get<1>(item);
    });
}

std::string ProgramConfig::read(NAME Key) {
    auto it = std::find_if(this->mapRegisterdName.begin(), this->mapRegisterdName.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName.end())
        return {};
    return this->mapConfigValue[it->second];
}

std::vector<std::string> ProgramConfig::read_lists(ProgramConfig::NAME Key) {
    auto                     v = read(Key);
    // split by ';' from single line
    std::stringstream        ss(v);
    std::string              item;
    std::vector<std::string> vecResult;
    while (std::getline(ss, item, ';')) {
        vecResult.push_back(item);
    }
    return vecResult;
}

bool ProgramConfig::write(NAME Key, const std::string &Value) {
    auto it = std::find_if(this->mapRegisterdName.begin(), this->mapRegisterdName.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName.end())
        return false;
    this->mapConfigValue[it->second] = Value;
    return true;
}

bool ProgramConfig::write_lists(ProgramConfig::NAME Key, const std::vector<std::string> &Value) {
    auto it = std::find_if(this->mapRegisterdName.begin(), this->mapRegisterdName.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName.end())
        return false;
    std::stringstream ss;
    std::for_each(Value.begin(), Value.end(), [&ss](const auto &item) {
        ss << item << ';';
    });
    this->mapConfigValue[it->second] = ss.str();
    return true;
}

bool ProgramConfig::refresh() {
    // clear config file content
    this->file.clear();
    this->file.seekg(0, std::ios::beg);

    // write config
    std::for_each(this->mapConfigValue.begin(), this->mapConfigValue.end(), [this](auto &item) {
        this->file << item.first << ":" << item.second << '\n';
    });

    return false;
}

ProgramConfig::~ProgramConfig() {
    refresh();
}
