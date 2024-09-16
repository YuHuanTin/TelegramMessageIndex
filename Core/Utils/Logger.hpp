//
// Created by AFETT on 2024/8/18.
//

#pragma once


namespace LogFormat {
    enum LogLevel : uint8_t {
        Debug   = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3
    };

    inline LogLevel                           MinOutputLevel = Debug;
    constexpr std::array<std::string_view, 4> LogTypeString  = { "debug", "Info", "Warning", "Error" };
    constexpr std::array<std::string_view, 5> LogColor       = {
        "\033[37m", // Debug    : white
        "\033[97m", // Info     : highlight white
        "\033[33m", // Warning  : yellow
        "\033[31m", // Error    : red
        "\033[0m"   // clean 
    };

    template<typename T>
    constexpr std::chrono::local_time<T> GetZoneTime() {
        const auto zoned_time_ = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
        return zoned_time_.get_local_time();
    }

    /**
     * 永远不要在 static 的构造函数中使用，否则初始化可能不及预期 
     */
    template<LogLevel Level, typename... Types>
    void LogFormatter(const std::format_string<Types...> Format, Types &&... Args) {
        if (Level < MinOutputLevel) {
            return;
        }
        std::println("{}[{:%Y-%m-%d %H:%M:%S}] [{}] {}{}",
            LogColor.at(Level),
            GetZoneTime<std::chrono::system_clock::duration>(),
            LogTypeString.at(Level),
            std::format(Format, std::forward<Types>(Args)...),
            LogColor.at(LogColor.size() - 1));
    }
}

