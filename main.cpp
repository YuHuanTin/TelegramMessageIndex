#include <print>

#include <concurrencpp/concurrencpp.h>
#include "Core/ObjectManager.h"

concurrencpp::result<void> DoLoop(TdClientCoreCo &TdClient) {
    ObjectManager object_manager { TdClient };
    while (true) {
        auto v = co_await TdClient.LoopIt(20);
        if (!v) {
            continue;
        }
        object_manager.ProcessObject(std::move(v));
    }
    co_return;
}

int main() {
    system("cls");
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));

    auto           startTimePoint = std::chrono::system_clock::now();
    TdClientCoreCo example;

    example.SendQuery(Utils::Make<td::td_api::addProxy>("127.0.0.1", 7890, true, Utils::Make<td::td_api::proxyTypeSocks5>()));
    example.Auth();
    std::println("elapsed time: {} ms", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTimePoint).count());

    DoLoop(example).get();
}
