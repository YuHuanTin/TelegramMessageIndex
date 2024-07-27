//
// Created by YuHuanTin on 2024/3/19.
//

#include "../Core/Register/StringRegister.h"
#include "../Core/Config/ProgramConfig.h"
#include <catch.hpp>
#include <filesystem>
#include <print>


void DeleteConfigFile() {
    // delete config file
    if (const std::filesystem::path fsys { std::filesystem::current_path() / "config" };
        std::filesystem::exists(fsys)) {
        std::println("find config file: {}, remove it", fsys.string());
        std::filesystem::remove(fsys);
    }
}

TEST_CASE("ProgramConfigTest", "[ProgramConfig R/W test]") {
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));

    DeleteConfigFile();
    ProgramConfig programConfig;

    SECTION("read and write") {
        // batch write
        programConfig.Write(REGISTER::STRING_POOL::config_proxy_host, "127.0.0.1");
        programConfig.Write(REGISTER::STRING_POOL::config_proxy_port, "8080");
        programConfig.Write(REGISTER::STRING_POOL::config_last_login_phone_number, "12345678901");
        programConfig.Refresh();

        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_proxy_host) == "127.0.0.1");
        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_proxy_port) == "8080");
        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_last_login_phone_number) == "12345678901");

        // single write
        programConfig.Write(REGISTER::STRING_POOL::config_proxy_host, "1.1.1.1");
        programConfig.Refresh();

        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_proxy_host) == "1.1.1.1");

        // write list
        programConfig.Write_lists(REGISTER::STRING_POOL::config_spy_picture_by_id_list, { "111111111", "2222222222", "dsaniu321" });
        programConfig.Refresh();

        auto l = programConfig.Read_lists(REGISTER::STRING_POOL::config_spy_picture_by_id_list);
        REQUIRE(l.size() == 3);
        REQUIRE(l[0] == "111111111");
        REQUIRE(l[1] == "2222222222");
        REQUIRE(l[2] == "dsaniu321");
    }

    SECTION("read empty") {
        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_proxy_host).empty());
        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_proxy_port).empty());
        REQUIRE(programConfig.Read(REGISTER::STRING_POOL::config_last_login_phone_number).empty());
    }
}
