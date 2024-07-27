//
// Created by YuHuanTin on 2024/3/19.
//

#include "ProgramConfig.h"

#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <string>
#include <vector>

#include "../Register/StringRegister.h"

ProgramConfig::ProgramConfig() {
    auto create_config_if_not_exist = [this] {
        const auto config_path = std::filesystem::current_path() / this->file_name_;
        if (exists(config_path)) {
            return;
        }
        std::fstream fs { config_path, std::ios::out };
        if (!fs.is_open()) {
            throw std::runtime_error(std::format(REGISTER::STRING_POOL::error_cannot_create_file, this->file_name_));
        }
        fs.close();
    };
    create_config_if_not_exist();


    // open config
    auto config_file_fs = std::fstream(this->file_name_, std::ios::in | std::ios::binary);
    if (!config_file_fs.is_open()) {
        throw std::runtime_error(std::format(REGISTER::STRING_POOL::error_cannot_open_file, this->file_name_));
    }

    // read config
    std::string                                       line;
    std::vector<std::pair<std::string, std::string> > vecConfigKV;
    while (std::getline(config_file_fs, line)) {
        if (const auto pos = line.find(':');
            pos != std::string::npos) {
            vecConfigKV.emplace_back(line.substr(0, pos), line.substr(pos + 1));
        } else {
            std::println(REGISTER::STRING_POOL::warn_invalid_config_line, line);
        }
    }


    // load config to memory map
    for (const auto &[k , v]: vecConfigKV) {
        this->mapConfigValue_[k] = v;
    }
}

std::string ProgramConfig::Read(std::string_view Key) {
    // only read need check key existence
    const auto it = mapConfigValue_.find(Key);
    if (it == this->mapConfigValue_.end())
        return {};
    return this->mapConfigValue_[it->first];
}

std::vector<std::string> ProgramConfig::Read_lists(std::string_view Key) {
    const auto result = Read(Key);
    if (result.empty())
        return {};
    return result | std::views::split(';') | std::ranges::to<std::vector<std::string> >();
}

bool ProgramConfig::Write(std::string_view Key, const std::string &Value) {
    this->mapConfigValue_[Key] = Value;
    return true;
}

bool ProgramConfig::Write_lists(std::string_view Key, const std::vector<std::string> &Value) {
    this->mapConfigValue_[Key] = Value | std::views::join_with(';') | std::ranges::to<std::string>();
    return true;
}

void ProgramConfig::Refresh() {
    // auto clean config file
    auto config_file_fs = std::fstream(this->file_name_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!config_file_fs.is_open()) {
        throw std::runtime_error(std::format(REGISTER::STRING_POOL::error_cannot_open_file, this->file_name_));
    }
    config_file_fs.seekp(0, std::ios::beg);


    // write config
    for (const auto &[k, v]: this->mapConfigValue_) {
        config_file_fs << k << ':' << v << '\n';
    }
}

ProgramConfig::~ProgramConfig() {
    Refresh();
}
