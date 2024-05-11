

#include <cstdio>
#include <print>
#include <iostream>

#include "Core/CoreWarpper.h"


std::tuple<std::string, int32_t> get_s5_proxy_from_input() {
    std::string s5_proxy_ip, s5_proxy_port;

    // query for socks5 proxy
    std::println("please input your socks5 proxy ip if exist (e.g. 127.0.0.1):");
    std::getline(std::cin, s5_proxy_ip);
    if (s5_proxy_ip.empty())
        return {};

    std::println("please input your socks5 proxy port (e.g. 11223):");
    std::getline(std::cin, s5_proxy_port);
    if (s5_proxy_port.empty())
        return {};

    if (!s5_proxy_ip.empty() && !s5_proxy_port.empty()) {
        return {s5_proxy_ip, std::stol(s5_proxy_port)};
    }
    return {};
}

int main() {
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));

    auto configService = std::make_shared<ProgramConfig>();
    auto proxyHost     = configService->read(REGISTER::CONFIG_STRING_NAME::proxy_host);
    auto proxyPort     = configService->read(REGISTER::CONFIG_STRING_NAME::proxy_port);

    if (!proxyHost.empty() && !proxyPort.empty()) {
        std::println("load config proxy host: {}\n"
            "load config proxy port: {}", proxyHost, proxyPort);
    } else {
        const auto [s5_proxy, s5_proxy_port] = get_s5_proxy_from_input();
        if (!s5_proxy.empty()) {
            configService->write(REGISTER::CONFIG_STRING_NAME::proxy_host, s5_proxy);
            configService->write(REGISTER::CONFIG_STRING_NAME::proxy_port, std::to_string(s5_proxy_port));
            configService->refresh();
            proxyHost = s5_proxy;
            proxyPort = std::to_string(s5_proxy_port);
        }
    }

    CoreWarpper example(configService, 0);
    if (!proxyHost.empty() && !proxyPort.empty()) {
        example.set_socks5_proxy(proxyHost, stol(proxyPort));
    }
    example.loop();
}
