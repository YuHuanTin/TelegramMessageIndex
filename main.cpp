
#include <concurrencpp/concurrencpp.h>
#include <td/telegram/td_api.h>

import Logger;
import ConsoleCtrlCapturer;
import TdUtils;
import ObjectManager;
import TelegramCore;
import StringRegister;
import ConfigRegister;


EagerRetType<> DoLoop(TdClientCore &TdClient) {
    ConsoleCtrlCapturer ctrl_c_capturer;
    ObjectManager       object_manager { TdClient };

    while (!ctrl_c_capturer.IsCaptureCtrlC()) {
        auto v = co_await TdClient.LoopIt(1);
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

    LogFormat::MinOutputLevel = REGISTER::CONFIGERS::Config_Log_Level.Value<LogFormat::LogLevel>();

    const auto   startTimePoint = std::chrono::system_clock::now();
    TdClientCore example;

    if (const auto &[enable, host, port] = REGISTER::CONFIGERS::Config_Proxy_Setting.Value<REGISTER::ProxySettings>();
        enable) {
        example.SendQuery(Utils::Make<td::td_api::addProxy>(host, port, true, Utils::Make<td::td_api::proxyTypeSocks5>()));
    }

    example.Auth();
    LogFormat::LogFormatter<LogFormat::Info>("elapsed time: {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTimePoint).count());

    DoLoop(example).get();
    LogFormat::LogFormatter<LogFormat::Info>("process end");
}
