

#include <cstdio>
#include <print>
#include <iostream>

#include "TelegramClientCore.h"


std::tuple<std::string, int32_t> get_s5_proxy_from_input() {
    // ask for socks5 proxy
    std::println("please input your socks5 proxy ip if exist (e.g. 127.0.0.1):");
    std::string s5_proxy_ip;
    std::getline(std::cin, s5_proxy_ip);
    if (s5_proxy_ip.empty()) return {};

    std::println("please input your socks5 proxy port (e.g. 11223):");
    std::string s5_proxy_port;
    std::getline(std::cin, s5_proxy_port);
    if (s5_proxy_port.empty()) return {};

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


    const auto [s5_proxy, s5_proxy_port] = get_s5_proxy_from_input();
    
    TelegramClientCore example;
    if (!s5_proxy.empty()) {
        example.set_proxy_s5(s5_proxy, s5_proxy_port);
    }
    example.loop();

}
