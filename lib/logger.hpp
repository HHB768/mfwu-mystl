#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include "list.hpp"
#include "string.hpp"

namespace mfwu {

std::chrono::high_resolution_clock::time_point start_time = {};

struct LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent() = default;
    LogEvent(const mfwu::string<char>& str)
        : file_(__FILE__), line_(rand() % 10), 
          elapse_(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start_time).count()),
          threadId_(rand() % 10), fiberId_(rand() % 10), time_(time(nullptr)) {
        content_ = str.copy();
    }
    void refresh(const mfwu::string<char>& str) {
        file_ = __FILE__;
        line_ = rand() % 10;
        elapse_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start_time).count();
        threadId_ = rand() % 10;
        fiberId_ = rand() % 10;
        time_ = time(nullptr);
        content_ = str.copy();
    }

// private:
    mfwu::string<char> file_ = {};
    int32_t line_ = 0;  // #line
    uint32_t elapse_ = 0;  // time cnt in ms
    uint32_t threadId_ = 0;
    uint32_t fiberId_ = 0;
    time_t time_ = 0;  // time stamp
    mfwu::string<char> content_;

};  // endof struct LogEvent

enum class LogLevel {
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
};  // endof enum class LogLevel

class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    LogFormatter() {}
    mfwu::string<char> format(LogEvent::ptr event) {
        std::stringstream ss;
        ss << "in file: " << event->file_ << ", ";
        ss << "line " << event->line_ << ":\n";
        ss << event->content_ << "\n";
        ss << "[ threadId: " << event->threadId_ << ", ";
        ss << "fiberId: " << event->fiberId_ << " ]\n";
        ss << ctime(&event->time_);
        ss << "elapse: " << event->elapse_ << "\n";
        return ss.str().c_str();  
        // std::string.c_str() differs from mfwu::string<char>
        // its retval is the ptr inside the string impl
        // so it may change after string change
        // and will be invalid after ~string()
    }
};  // endof class LogFormatter

class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    LogAppender(LogLevel level=LogLevel::DEBUG) 
        : level_(level), formatter_(new LogFormatter()) {}
    virtual ~LogAppender() {}
    virtual void append(LogLevel level, LogEvent::ptr event) = 0;
    mfwu::string<char> format(LogEvent::ptr event) {
         return formatter_->format(event);
    }
    LogLevel get_level() const { return level_; }
    void set_level(LogLevel level) { level_ = level; }
private:
    LogLevel level_;
    LogFormatter::ptr formatter_;
};  // endof LogAppender

class StdoutLogAppender : public LogAppender {
public:
    StdoutLogAppender(LogLevel level=LogLevel::DEBUG)
        : LogAppender(level) {}
    void append(LogLevel level, LogEvent::ptr event) {
        if (level >= get_level()) {
#ifdef __UNIT_TEST_LOGGER__
#ifndef __UNIT_TEST_LOGGER_BRIEF__
            std::cout << format(event) << "\n";
#else  // !UNIT_TEST_LOGGER_BRIEF__
            format(event);
#endif  // UNIT_TEST_LOGGER_BRIEF__
#endif  // UNIT_TEST_LOGGER__
        }
    }
private:
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender(LogLevel level=LogLevel::DEBUG, 
                    const mfwu::string<char> output_dir="./log.out")
        : LogAppender(level), output_dir_(output_dir) {}
    void append(LogLevel level, LogEvent::ptr event) {
        if (level >= get_level()) {
            std::fstream fs;
#ifdef __UNIT_TEST_LOGGER__
#ifndef __UNIT_TEST_LOGGER_BRIEF__
            fs.open(output_dir_.c_str(), std::ios_base::app);
            fs << format(event) << "\n";
#else  // !UNIT_TEST_LOGGER_BRIEF__
            format(event);
#endif  // UNIT_TEST_LOGGER_BRIEF__
#endif  // UNIT_TEST_LOGGER__
            // fs.close();  // dont need this
                            // included in ~fstream
        }
    }
private:
    mfwu::string<char> output_dir_;
};

class Logger {
public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const mfwu::string<char>& name="root")
        : name_(), level_(LogLevel::DEBUG), appenders_() {}
    void log(LogLevel level, LogEvent::ptr event) {
        for (auto appender : appenders_) {
            appender->append(level, event);
        }
    }

    void debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }
    void info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }
    void warn(LogEvent::ptr event){
        log(LogLevel::WARN, event);
    }
    void error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }
    void fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }

    void addAppender(bool mode=0, 
                     LogLevel level=LogLevel::DEBUG, 
                     const mfwu::string<char>& filename="") {
        if (mode) {
            appenders_.push_front(std::shared_ptr<LogAppender>(new FileLogAppender(level, filename)));  // TODO
        } else {
            appenders_.push_front(std::shared_ptr<LogAppender>(new StdoutLogAppender(level)));
        }
    }
    void delAppender() {
        appenders_.pop_front();
    }
private:
    mfwu::string<char> name_;  // ?
    LogLevel level_;  // ?
    mfwu::list<LogAppender::ptr> appenders_;
};  // endof class Logger

}  // endof namespace mfwu

#endif  // __LOGGER_HPP__