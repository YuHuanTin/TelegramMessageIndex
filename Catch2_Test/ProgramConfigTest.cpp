//
// Created by YuHuanTin on 2024/3/19.
//


#include <catch.hpp>

#include "../Core/Register/StringRegister.h"
#include "../Core/Config/StorageManager2.h"

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

    StorageManager2 storage_manager;
    storage_manager.Write("test", 1123);
    storage_manager.Write("dasjoidsa", "dsadsa");
    
    storage_manager.SyncFile();

    auto v = storage_manager.Read<int>("test");
    auto v2 = storage_manager.Read<std::string>("dasjoidsa");

    std::println("{}, {}", v, v2);
}
