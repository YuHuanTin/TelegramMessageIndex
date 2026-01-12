//
// Created by YuHuanTin on 2026/1/12.
//

module;

#include <nlohmann/json.hpp>

export module Configs;

import Logger;
import std;

export namespace CONFIGS
{
    struct ProxySettings {
        bool          enable { false };
        std::string   host;
        std::uint32_t port { 0 };
        std::string   username;
        std::string   password;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ProxySettings, enable, host, port, username, password)
    };

    struct Logs {
        LogFormat::LogLevel log_level { LogFormat::Debug };
        std::string         redirect_file; // if empty, do not redirect

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Logs, log_level, redirect_file)
    };

    struct Root {
        ProxySettings proxy;
        Logs          logs;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Root, proxy, logs)
    };

    ProxySettings global_proxy_settings;
    Logs          global_logs_settings;

    void LoadConfig() {
        const std::string config_path = "config.json";

        try {
            std::ifstream file_stream(config_path);

            if (!file_stream.is_open()) {
                std::println("[warning] Config file not found, creating default.");
                std::ofstream  out(config_path);
                nlohmann::json j;
                j["proxy"] = global_proxy_settings;
                j["logs"]  = global_logs_settings;
                out << j.dump(4);
                return;
            }

            nlohmann::json json_data;
            file_stream >> json_data;

            Root config           = json_data.get<Root>();
            global_proxy_settings = config.proxy;
            global_logs_settings  = config.logs;

            std::println("[info] Config loaded successfully.");
        } catch (const std::exception &e) {
            std::println("[error] Failed to parse config: {}, using defaults.", e.what());
        }
    }
}
