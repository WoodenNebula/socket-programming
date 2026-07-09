#pragma once

#include <iostream>
#include <format>
#include <sstream>
#include <source_location>

constexpr const char* COLOR_RESET = "\033[0m";
constexpr const char* COLOR_WHITE = "\033[97m";
constexpr const char* COLOR_RED = "\033[91m";
constexpr const char* COLOR_GREEN = "\033[32m";
constexpr const char* COLOR_GREEN_LIGHT = "\033[92m";
constexpr const char* COLOR_YELLOW = "\033[93m";
constexpr const char* COLOR_GRAY = "\033[90m";
constexpr const char* COLOR_CYAN = "\033[96m";

namespace Engine
{
enum class ELogLevel
{
    Trace = 0,
    Success,
    Info,
    Warn,
    Error,
    Fatal
};


#ifdef DEBUG
#define ENGINE_LOG_LEVEL Engine::ELogLevel::Warn
#else
#define ENGINE_LOG_LEVEL Engine::ELogLevel::Info
#endif

struct LogCategory
{
    std::string_view Name;
    ELogLevel LogLevel;

    LogCategory(const std::string_view name, ELogLevel level = ENGINE_LOG_LEVEL)
        : Name(name), LogLevel(level)
    {}
};

inline static void LogWrite(const LogCategory& category, ELogLevel level, std::string_view msg, const std::source_location& location = std::source_location::current())
{
    if (level < ENGINE_LOG_LEVEL)
        return;
    if (level < category.LogLevel)
        return;

    std::stringstream logHeader;

    switch (level)
    {
    case ELogLevel::Trace:      logHeader << COLOR_GRAY << "[TRACE] - ";        break;
    case ELogLevel::Success:    logHeader << COLOR_GREEN << "[SUCCESS] - ";     break;
    case ELogLevel::Info:       logHeader << COLOR_WHITE << "[INFO] - ";        break;
    case ELogLevel::Warn:       logHeader << COLOR_YELLOW << "[WARNING] - ";    break;

    case ELogLevel::Error:
        logHeader << COLOR_RED
            << location.file_name() << "\n"
            << "\t" << "[" << location.line() << "] " << location.function_name() << "\n"
            << "\t[ERROR] - ";
        break;

    case ELogLevel::Fatal:
        logHeader << COLOR_RED
            << location.file_name() << "\n"
            << "\t" << "[" << location.line() << "] " << location.function_name() << "\n"
            << "\t[FATAL] - ";
        break;

    default: break;
    }

    if (level == ELogLevel::Fatal)
    {
        std::cerr << logHeader.str()
            << "[" << category.Name << "] "
            << msg << COLOR_RESET << "\n";
        exit(1);
    }
    else
    {
        std::cout << logHeader.str()
            << "[" << category.Name << "] "
            << msg << COLOR_RESET << "\n";
    }
}
#define DECLARE_LOG_CATEGORY(CategoryName) \
inline Engine::LogCategory Log##CategoryName{#CategoryName};

#define DECLARE_LOG_CATEGORY_LEVEL(CategoryName, Level) \
inline Engine::LogCategory Log##CategoryName{#CategoryName, Level };

#define LOG(Category, Level, Fmt, ...) \
    do { \
        Engine::LogWrite( \
            Category, \
            Level, \
            std::format(Fmt __VA_OPT__(,) __VA_ARGS__) \
        ); \
    } while (0)
}; // namespace Engine

inline constexpr Engine::ELogLevel Trace = Engine::ELogLevel::Trace;
inline constexpr Engine::ELogLevel Success = Engine::ELogLevel::Success;
inline constexpr Engine::ELogLevel Info = Engine::ELogLevel::Info;
inline constexpr Engine::ELogLevel Warning = Engine::ELogLevel::Warn;
inline constexpr Engine::ELogLevel Error = Engine::ELogLevel::Error;
inline constexpr Engine::ELogLevel Fatal = Engine::ELogLevel::Fatal;

template <typename... Args>
void Print(std::format_string<Args...> fmt, Args&&... args)
{
    std::cout << std::format(fmt, std::forward<Args>(args)...) << std::endl;
}

DECLARE_LOG_CATEGORY(Any);
