

#include <cstdio>

#include "TelegramClientCore.h"

int main() {
    // disable output buffering
    setvbuf(stdout, nullptr, _IONBF, 0);

    // setting global encoding utf-8
    std::locale::global(std::locale("zh_CN.UTF-8"));


    TelegramClientCore example;
    example.set_proxy_s5("127.0.0.1", 11223);
    example.loop();


}
