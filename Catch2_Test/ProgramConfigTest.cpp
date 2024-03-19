//
// Created by YuHuanTin on 2024/3/19.
//

#include "../Config/ProgramConfig.h"
#include <catch.hpp>
#include <print>

TEST_CASE("ReadProgramConfigTest", "[ProgramConfig]") {
    ProgramConfig programConfig;

    auto proxyHost            = programConfig.read(ProgramConfig::proxy_host);
    auto proxyPort            = programConfig.read(ProgramConfig::proxy_port);
    auto lastLoginPhoneNumber = programConfig.read(ProgramConfig::last_login_phone_number);

    std::println("proxyHost: {}, proxyPort: {}, lastLoginPhoneNumber: {}", proxyHost, proxyPort, lastLoginPhoneNumber);
}

TEST_CASE("WriteProgramConfigTest", "[ProgramConfig]") {
    ProgramConfig programConfig;
    programConfig.write(ProgramConfig::proxy_host, "127.0.0.1");
    programConfig.write(ProgramConfig::proxy_port, "8080");
    programConfig.write(ProgramConfig::last_login_phone_number, "12345678901");

    std::println("proxyHost: {}, proxyPort: {}, lastLoginPhoneNumber: {}", programConfig.read(ProgramConfig::proxy_host),
                 programConfig.read(ProgramConfig::proxy_port), programConfig.read(ProgramConfig::last_login_phone_number));
}