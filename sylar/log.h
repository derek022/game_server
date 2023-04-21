#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <memory>
#include <stdint.h>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <fstream>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "singleton.h"
#include "thread.h"

#define SYLAR_LOG_LEVEL(logger, level)                                                                        \
    if (logger->getLevel() <= level)                                                                          \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level,                               \
                                                                 __FILE__, __LINE__, 0, sylar::GetThreadId(), \
                                                                 sylar::GetFiberId(), time(0), sylar::Thread::GetName())))     \
        .getSS()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                                                          \
    if (logger->getLevel() <= level)                                                                          \
    sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level,                               \
                                                                 __FILE__, __LINE__, 0, sylar::GetThreadId(), \
                                                                 sylar::GetFiberId(), time(0), sylar::Thread::GetName())))     \
        .getEvent()                                                                                           \
        ->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar
{
    class Logger;
    class LoggerManager;
    // 日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            /// 未知级别
            UNKNOW = 0,
            /// DEBUG 级别
            DEBUG = 1,
            /// INFO 级别
            INFO = 2,
            /// WARN 级别
            WARN = 3,
            /// ERROR 级别
            ERROR = 4,
            /// FATAL 级别
            FATAL = 5
        };
        static const char *ToString(LogLevel::Level level);
        static LogLevel::Level FromString(const std::string& str);
    };

    // 日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse,
                 uint32_t thread_id, uint32_t fiber_id, uint64_t time,
                 const std::string& thread_name);

        const char *getFile() const { return m_file; }
        int32_t getLine() const { return m_line; }
        uint32_t getElapse() const { return m_elapse; }
        uint32_t getThreadId() const { return m_threadId; }
        uint32_t getFiberId() const { return m_fiberId; }
        uint64_t getTime() const { return m_time; }
        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        const std::string& getThreadName() const { return m_threadName; }
        /**
         * @brief 返回日志内容
         */
        std::string getContent() const { return m_ss.str(); }

        std::stringstream& getSS() { return m_ss; }
        LogLevel::Level getLevel() const { return m_level; }
        /**
         * @brief 格式化写入日志内容
         */
        void format(const char *fmt, ...);

        /**
         * @brief 格式化写入日志内容
         */
        void format(const char *fmt, va_list al);

    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0;           // 行号
        uint32_t m_elapse = 0;        // 程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;      // 线程 ID
        uint32_t m_fiberId = 0;       // 携程 ID
        uint64_t m_time;              // 时间戳
        std::string m_content;
        /// 线程名称
        std::string m_threadName;
        /// 日志内容流
        std::stringstream m_ss;
        /// 日志器
        std::shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;
    };

    class LogEventWrap
    {
    public:
        LogEventWrap(LogEvent::ptr e);

        ~LogEventWrap();

        LogEvent::ptr getEvent() const { return m_event; }

        std::stringstream& getSS();

    private:
        LogEvent::ptr m_event;
    };

    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);

        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    public:
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

    public:
        void init();

        bool isError() const { return m_error; }

        const std::string getPattern() const { return m_pattern; }

    private:
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> m_items;
        bool m_error = false;
    };

    class LogAppender
    {
        friend class Logger;

    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef NullMutex MutexType;

        virtual ~LogAppender() {}
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        virtual std::string toYamlString() = 0;

        void setFormatter(LogFormatter::ptr val);
        LogFormatter::ptr getFormatter();

        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level val) { m_level = val; }

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        bool m_hasFormatter = false;
        MutexType m_mutex;
        LogFormatter::ptr m_formatter;
    };

    class Logger : public std::enable_shared_from_this<Logger>
    {
	friend class LoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef NullMutex MutexType;

        Logger(const std::string& name = "root");
        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        void clearAppenders();

        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level val) { m_level = val; }

        const std::string& getName() const { return m_name; }

        void setFormatter(LogFormatter::ptr val);

        void setFormatter(const std::string& val);

        LogFormatter::ptr getFormatter();

        std::string toYamlString();

    private:
        std::string m_name;
        LogLevel::Level m_level;
        // mutex
        MutexType m_mutex;
        std::list<LogAppender::ptr> m_appenders;
        LogFormatter::ptr m_formatter;
        Logger::ptr m_root;
    };

    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;

    private:
    };

    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& fileName);

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;

        bool reopen();

    private:
        std::string m_filename;
        std::ofstream m_filestream;
        uint64_t m_lastTime = 0;
    };

    class LoggerManager
    {
    public:
        typedef NullMutex MutexType;

        LoggerManager();

        Logger::ptr getLogger(const std::string& name);
        Logger::ptr getRoot() const { return m_root; }
        void init();

        std::string toYamlString();

    private:
        MutexType m_mutex;
        std::map<std::string, Logger::ptr> m_loggers;
        Logger::ptr m_root;
    };

    typedef sylar::Singleton<LoggerManager> LoggerMgr;

}

#endif
