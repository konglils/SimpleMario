//
// Created by MINEC on 2026/5/10.
//

// 由 AI 编写

#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <source_location>
#include <filesystem>
#include <format>
#ifdef _WIN32
#include <windows.h>
#endif

// ANSI 终端颜色代码
namespace ColorCode {
    inline const char* RESET      = "\033[0m";
    inline const char* RED        = "\033[31m";
    inline const char* BOLD_RED   = "\033[91m";  // 高亮红，更醒目，适合 ERROR
    inline const char* YELLOW     = "\033[33m";  // 黄色，适合 WARN
    inline const char* GREEN      = "\033[32m";  // 绿色，适合 INFO
}

// ============================================================================
// 日志级别枚举
// ============================================================================
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5
};

// ============================================================================
// 编译期日志级别配置 (用于发布版本零开销)
// ============================================================================
// 在发布版本中，你可以把这里改成 Info 或 Warn，这样低于该级别的日志
// 在编译时就会被完全删除，连参数求值都不会有，实现真正的零开销。
#ifndef LOG_LEVEL_COMPILE_TIME
    #define LOG_LEVEL_COMPILE_TIME LogLevel::Trace // 默认开发期显示所有
#endif

namespace detail {
    // 编译期级别判断辅助函数
    constexpr bool compileTimeLevelEnabled(LogLevel level) {
        return level >= LOG_LEVEL_COMPILE_TIME;
    }
}

// ============================================================================
// 日志系统核心类 (单例模式)
// ============================================================================
class Logger {
public:
    // 获取单例实例 (Meyer's Singleton，C++11 保证线程安全)
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // 禁止拷贝和移动
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // 设置运行时日志级别
    void setLogLevel(LogLevel level) {
        logLevel = level;
    }

    // 获取当前运行时日志级别
    [[nodiscard]] LogLevel getLogLevel() const {
        return logLevel;
    }

    // 开启/关闭日志系统 (替代之前的 LogLevel::Off)
    void enableLogging(bool enabled) {
        loggingEnabled = enabled;
    }

    // 设置日志文件路径
    void setLogFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open()) {
            logFile.close();
        }

        // 确保目录存在 (使用 std::error_code 版本，防止路径异常时抛出异常)
        std::filesystem::path filePath(path);
        if (filePath.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(filePath.parent_path(), ec);
            if (ec) {
                std::cerr << "Failed to create log directories: " << ec.message() << std::endl;
            }
        }

        logFile.open(path, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << path << std::endl;
        }
    }

    // 关闭日志文件
    void closeLogFile() {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // ========================================================================
    // 核心日志记录方法 (无格式化，接受 std::string_view)
    // ========================================================================
    void log(LogLevel level,
             std::string_view message,
             const std::source_location& location = std::source_location::current())
    {
        // 运行时级别过滤或开关关闭
        if (!loggingEnabled || level < logLevel) return;

        std::lock_guard<std::mutex> lock(logMutex);

        // 1. 获取当前时间 (带毫秒)
        const auto now = std::chrono::system_clock::now();
        const auto now_time_t = std::chrono::system_clock::to_time_t(now);
        const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;

        // 线程安全的时间转换
        std::tm tm_buf;
        #ifdef _WIN32
            localtime_s(&tm_buf, &now_time_t); // Windows
        #else
            localtime_r(&now_time_t, &tm_buf); // Linux/Mac
        #endif

        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << now_ms.count();

        // 2. 日志级别
        ss << " [" << levelToString(level) << "] ";

        // 3. 位置信息 (提取短文件名)
        std::string file_name = location.file_name();
        const size_t last_slash = file_name.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            file_name = file_name.substr(last_slash + 1);
        }

        ss << "[" << file_name << ":" << location.line() << "] ";
        ss << "[" << location.function_name() << "] ";

        // 4. 消息内容
        ss << message << std::endl;

        const std::string logMessage = ss.str();

        // 5. 输出到控制台 (根据级别加颜色)
        if (level == LogLevel::Error || level == LogLevel::Critical) {
            std::cout << ColorCode::BOLD_RED << logMessage << ColorCode::RESET;
        } else if (level == LogLevel::Warn) {
            std::cout << ColorCode::YELLOW << logMessage << ColorCode::RESET;
        } else if (level == LogLevel::Info) {
            std::cout << ColorCode::GREEN << logMessage << ColorCode::RESET; // Info顺手上个绿色
        } else {
            std::cout << logMessage; // Trace/Debug 保持默认色
        }
        std::cout.flush();

        // 6. 输出到文件 (千万不要带颜色代码，否则文件里全是乱码)
        if (logFile.is_open()) {
            logFile << logMessage;
            logFile.flush();
        }
    }

    // ========================================================================
    // 核心日志记录方法 (支持格式化，C++20 std::format)
    // ========================================================================
    // 【关键修复】：将 source_location 移到 Args... 前面！
    // 如果放在后面，编译器会尝试把 fmt 后面的参数(比如 clients.size())
    // 解析为 source_location，导致 "Cannot convert rvalue of type std::size_t
    // to parameter type std::source_location" 错误。
    template<typename... Args>
    void log_fmt(LogLevel level,
                 const std::source_location& location, // 位置提前！且不用默认参数
                 std::format_string<Args...> fmt,
                 Args&&... args)
    {
        // 先格式化字符串，再调用无格式化的 log 方法
        std::string formatted_msg = std::format(fmt, std::forward<Args>(args)...);
        log(level, formatted_msg, location);
    }

    // ========================================================================
    // 便捷方法：直接记录字符串 (无格式化)
    // ========================================================================
    void trace(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Trace, message, location);
    }
    void debug(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Debug, message, location);
    }
    void info(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Info, message, location);
    }
    void warn(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Warn, message, location);
    }
    void error(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Error, message, location);
    }
    void critical(std::string_view message, const std::source_location& location = std::source_location::current()) {
        log(LogLevel::Critical, message, location);
    }

private:
    Logger() : logLevel(LogLevel::Info), loggingEnabled(true) {
        // Windows 平台开启 ANSI 颜色支持
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // 启用虚拟终端
            SetConsoleMode(hOut, dwMode);
        }
#endif
    }
    ~Logger() { closeLogFile(); }

    [[nodiscard]] const char* levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace:    return "TRACE";
            case LogLevel::Debug:    return "DEBUG";
            case LogLevel::Info:     return "INFO ";
            case LogLevel::Warn:     return "WARN ";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRIT ";
            default:                 return "UNKN ";
        }
    }

    LogLevel logLevel;
    bool loggingEnabled;
    std::ofstream logFile;
    std::mutex logMutex;
};

// ===========================================================================
// 宏定义部分
// ===========================================================================
// 双重检查：
// 1. if constexpr：编译期判断，级别不够代码直接消失（零开销）。
// 2. if：运行时判断，支持热更新级别。

// 基础日志宏 (无格式化)
#define LOG_TRACE(msg)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Trace))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Trace)    Logger::getInstance().trace(msg); } } while(0)
#define LOG_DEBUG(msg)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Debug))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Debug)    Logger::getInstance().debug(msg); } } while(0)
#define LOG_INFO(msg)     do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Info))     { if (Logger::getInstance().getLogLevel() <= LogLevel::Info)     Logger::getInstance().info(msg); } } while(0)
#define LOG_WARN(msg)     do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Warn))     { if (Logger::getInstance().getLogLevel() <= LogLevel::Warn)     Logger::getInstance().warn(msg); } } while(0)
#define LOG_ERROR(msg)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Error))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Error)    Logger::getInstance().error(msg); } } while(0)
#define LOG_CRITICAL(msg) do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Critical)) { if (Logger::getInstance().getLogLevel() <= LogLevel::Critical) Logger::getInstance().critical(msg); } } while(0)

// 格式化日志宏 (使用 C++20 std::format)
// 【关键修复】：在宏里显式传入 std::source_location::current()，这样就避开了可变参数解析错误！
#define LOG_TRACE_FMT(fmt, ...)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Trace))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Trace)    Logger::getInstance().log_fmt(LogLevel::Trace,    std::source_location::current(), fmt, __VA_ARGS__); } } while(0)
#define LOG_DEBUG_FMT(fmt, ...)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Debug))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Debug)    Logger::getInstance().log_fmt(LogLevel::Debug,    std::source_location::current(), fmt, __VA_ARGS__); } } while(0)
#define LOG_INFO_FMT(fmt, ...)     do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Info))     { if (Logger::getInstance().getLogLevel() <= LogLevel::Info)     Logger::getInstance().log_fmt(LogLevel::Info,     std::source_location::current(), fmt, __VA_ARGS__); } } while(0)
#define LOG_WARN_FMT(fmt, ...)     do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Warn))     { if (Logger::getInstance().getLogLevel() <= LogLevel::Warn)     Logger::getInstance().log_fmt(LogLevel::Warn,     std::source_location::current(), fmt, __VA_ARGS__); } } while(0)
#define LOG_ERROR_FMT(fmt, ...)    do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Error))    { if (Logger::getInstance().getLogLevel() <= LogLevel::Error)    Logger::getInstance().log_fmt(LogLevel::Error,    std::source_location::current(), fmt, __VA_ARGS__); } } while(0)
#define LOG_CRITICAL_FMT(fmt, ...) do { if constexpr (detail::compileTimeLevelEnabled(LogLevel::Critical)) { if (Logger::getInstance().getLogLevel() <= LogLevel::Critical) Logger::getInstance().log_fmt(LogLevel::Critical, std::source_location::current(), fmt, __VA_ARGS__); } } while(0)



/*

// 使用示例
#include "Logger.hpp" // 假设上面的代码保存在这个文件里

struct Player {
    int id;
    std::string name;
};

// 为自定义类型特化 std::formatter (C++20 特性，支持直接格式化输出)
template <>
struct std::formatter<Player> : std::formatter<std::string> {
    auto format(Player p, format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("Player(id={}, name={})", p.id, p.name), ctx);
    }
};

int main() {
    // 1. 初始化日志系统
    Logger::getInstance().setLogFile("logs/game.log");
    Logger::getInstance().setLogLevel(LogLevel::Debug); // 只显示 Debug 及以上级别

    // 2. 使用基础宏
    LOG_INFO("游戏服务器启动...");
    LOG_DEBUG("加载配置文件完成");

    // 3. 使用格式化宏 (非常高效且安全)
    Player p1 = {1, "Alice"};
    LOG_INFO_FMT("玩家 {} 登录游戏", p1);
    LOG_WARN_FMT("内存占用过高: {} MB", 1024);

    // 4. 故意写一个错误日志，观察文件名和行号
    LOG_ERROR("发生了一个严重的错误！");

    return 0;
}

*/