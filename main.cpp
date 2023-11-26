

#include <cstdio>

#include "TelegramClientCore.h"

int main() {

    setbuf(stdout, nullptr);

    TelegramClientCore example;
    example.set_proxy_s5("127.0.0.1", 11223);
    example.loop();


}
