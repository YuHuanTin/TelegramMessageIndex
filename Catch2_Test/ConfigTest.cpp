//
// Created by AFETT on 2024/7/27.
//

#include <iostream>
#include <print>

#include "../Core/Register/ConfigRegister.h"
#include "../Core/Utils/Logger.h"
#include "catch2/catch_all.hpp"


TEST_CASE("test config") {
    // std::ios::sync_with_stdio(false);
    // std::cin.tie(nullptr);
    // std::cout.tie(nullptr);


    std::println("{}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());
    REGISTER::CONFIGERS::Config_Proxy_Host.Set_Value("dsadsadsa.1.1.1");
    std::println("{}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());

    SECTION("se1") {
        std::println("se1 {}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());
        REGISTER::CONFIGERS::Config_Proxy_Host.Set_Value("dsadsadsa.1.1.1");
        std::println("se1 {}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());
    }

    SECTION("se2") {
        std::println("se2 {}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());
        REGISTER::CONFIGERS::Config_Proxy_Host.Set_Value("dsadsadsa.1.1.1");
        std::println("se2 {}", REGISTER::CONFIGERS::Config_Proxy_Host.Value());
    }
}
