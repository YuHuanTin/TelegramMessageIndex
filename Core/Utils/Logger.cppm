
module;

#include <chrono>
#include <cstdint>

export module Logger;

export import std;

export namespace LogFormat
{
    enum LogLevel : uint8_t {
        Debug   = 0,
        Info    = 1,
        Warning = 2,
        Error   = 3
    };

    inline LogLevel                           MinOutputLevel = Debug;
    std::string                               redirect_file;
    constexpr std::array<std::string_view, 4> LogTypeString = { "debug", "Info", "Warning", "Error" };
    constexpr std::array<std::string_view, 5> LogColor      = {
        "\033[37m", // Debug    : white
        "\033[97m", // Info     : highlight white
        "\033[33m", // Warning  : yellow
        "\033[31m", // Error    : red
        "\033[0m"   // clean
    };

    void Init(LogLevel log_setting, std::string file = "") {
        LogFormat::MinOutputLevel = log_setting;
        LogFormat::redirect_file  = std::move(file);
    }

    /**
     * 永远不要在 static 的构造函数中使用，否则初始化可能不及预期
     */
    template<LogLevel Level, typename... Types>
    void LogFormatter(const std::format_string<Types...> Format, Types &&...Args) {
        if (Level < MinOutputLevel) {
            return;
        }

        std::string formatedText = std::format("{}[{:%Y-%m-%d %H:%M:%S}] [{}] {}{}",
            LogColor.at(Level),
            std::chrono::system_clock::now(),
            LogTypeString.at(Level),
            std::format(Format, std::forward<Types>(Args)...),
            LogColor.at(LogColor.size() - 1));

        if (!LogFormat::redirect_file.empty()) {
            static std::ofstream log_file_stream(LogFormat::redirect_file, std::ios::app);
            if (log_file_stream.is_open()) {
                log_file_stream << formatedText << std::endl;
            } else {
                std::println("warn: cannot open log file: {}", LogFormat::redirect_file);
            }
        } else {
            std::println("{}", formatedText);
        }
    }
}
