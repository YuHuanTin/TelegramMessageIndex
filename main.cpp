
#include <concurrencpp/concurrencpp.h>
#include <td/telegram/td_api.h>

import Logger;
import ConsoleCtrlCapturer;
import TdUtils;
import ObjectManager;
import TelegramCore;
import Configs;


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

    // init configs
    CONFIGS::LoadConfig();

    // init log formatter
    LogFormat::Init(CONFIGS::global_logs_settings.log_level, CONFIGS::global_logs_settings.redirect_file);


    const auto   startTimePoint = std::chrono::system_clock::now();
    TdClientCore core;

    if (CONFIGS::global_proxy_settings.enable) {
        auto s5       = Utils::Make<td::td_api::proxyTypeSocks5>(CONFIGS::global_proxy_settings.username, CONFIGS::global_proxy_settings.password);
        auto addProxy = Utils::Make<td::td_api::addProxy>(CONFIGS::global_proxy_settings.host, CONFIGS::global_proxy_settings.port, true, std::move(s5));
        core.SendQuery(std::move(addProxy));
    }

    core.Auth();
    LogFormat::LogFormatter<LogFormat::Info>("elapsed time: {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTimePoint).count());

    DoLoop(core).get();
    LogFormat::LogFormatter<LogFormat::Info>("process end");
}
