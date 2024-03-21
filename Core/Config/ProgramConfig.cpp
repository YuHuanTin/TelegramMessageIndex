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
    this->file_.open("config");
    if (!this->file_.is_open()) {
        this->file_.clear();
        this->file_.open("config", std::ios::out); // 创建文件
        if (!this->file_.is_open()) {
            std::println("Can not create config file");
            return;
        }
        this->file_.close();
        this->file_.open("config", std::ios::in | std::ios::out | std::ios::binary);
        if (!this->file_.is_open()) {
            std::println("Can not open config file");
            return;
        }
    }

    // read config
    std::string                                      line;
    std::vector<std::pair<std::string, std::string>> vecConfigKV;
    while (std::getline(this->file_, line)) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            vecConfigKV.emplace_back(line.substr(0, pos), line.substr(pos + 1));
        }
    }

    std::for_each(vecConfigKV.begin(), vecConfigKV.end(), [this](auto &item) {
        this->mapConfigValue_[item.first] = item.second;
    });
}

std::string ProgramConfig::read(REGISTER::CONFIG_STRING_NAME Key) {
    auto it = std::find_if(this->mapRegisterdName_.mapRegisterdStr_.begin(), this->mapRegisterdName_.mapRegisterdStr_.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName_.mapRegisterdStr_.end())
        return {};
    return this->mapConfigValue_[it->second];
}

std::vector<std::string> ProgramConfig::read_lists(REGISTER::CONFIG_STRING_NAME Key) {
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

bool ProgramConfig::write(REGISTER::CONFIG_STRING_NAME Key, const std::string &Value) {
    auto it = std::find_if(this->mapRegisterdName_.mapRegisterdStr_.begin(), this->mapRegisterdName_.mapRegisterdStr_.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName_.mapRegisterdStr_.end())
        return false;
    this->mapConfigValue_[it->second] = Value;
    return true;
}

bool ProgramConfig::write_lists(REGISTER::CONFIG_STRING_NAME Key, const std::vector<std::string> &Value) {
    auto it = std::find_if(this->mapRegisterdName_.mapRegisterdStr_.begin(), this->mapRegisterdName_.mapRegisterdStr_.end(), [Key](auto &item) {
        return item.first == Key;
    });
    if (it == this->mapRegisterdName_.mapRegisterdStr_.end())
        return false;
    std::stringstream ss;
    std::for_each(Value.begin(), Value.end(), [&ss](const auto &item) {
        ss << item << ';';
    });
    this->mapConfigValue_[it->second] = ss.str();
    return true;
}

bool ProgramConfig::refresh() {
    // clear config file content
    this->file_.clear();
    this->file_.seekg(0, std::ios::beg);

    // write config
    std::for_each(this->mapConfigValue_.begin(), this->mapConfigValue_.end(), [this](auto &item) {
        this->file_ << item.first << ":" << item.second << '\n';
    });

    return true;
}

ProgramConfig::~ProgramConfig() {
    refresh();
}
