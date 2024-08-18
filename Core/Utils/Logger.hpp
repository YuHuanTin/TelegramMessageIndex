//
// Created by AFETT on 2024/8/18.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <array>
#include <chrono>
#include <string_view>

namespace LogFormat {
    enum LogLevel : uint8_t {
        Debug   = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3
    };

    inline std::array<std::string_view, 4> LogTypeString = { "debug", "Info", "Warning", "Error" };
    inline std::array<std::string_view, 5> LogColor      = {
        "\033[1;45m", // Debug: highlight purple
        "\033[1;37m", // Info: highlight white
        "\033[1;33m", // Warning: yellow
        "\033[1;31m", // Error: red
        "\033[0m"     // clean 
    };

    template<typename T>
    constexpr std::chrono::local_time<T> getZoneTime() {
        const auto zoned_time_ = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
        return zoned_time_.get_local_time();
    }

    template<LogLevel Level, typename... Types>
    void LogFormatter(const std::format_string<Types...> Format, Types &&... Args) {
#ifdef _DEBUG
        std::println("{}[{:%Y-%m-%d %H:%M:%S}] [{}] {}{}",
            LogFormat::LogColor.at(Level),
            getZoneTime<std::chrono::system_clock::duration>(),
            LogFormat::LogTypeString[Level],
            std::format(Format, std::forward<Types>(Args)...),
            LogFormat::LogColor.at(4));
#endif
    }
}


#endif //LOGGER_HPP
