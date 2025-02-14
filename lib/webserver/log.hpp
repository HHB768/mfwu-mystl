#ifndef __LOG_HPP__
#define __LOG_HPP__


#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "blockqueue.h"
#include "buffer.hpp"

namespace mfwu_webserver {

class Log {
public:
    void init(int level, const char* path="./log",
              const char* suffix=".log",
              int maxQueueCapacity=1024);
    static Log* Instance();
    static void FlushLogThread();

    void write(int level, const char* format, ...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return isOpen_; }

private:
    Log();
    void AppendLogLevelTitle(int level);
    virtual ~Log();
    void AsyncWrite_();

    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char* path_;
    const char* suffix_;

    int MAX_LINES_;
    int lineCount_;
    int toDay_;
    bool isOpen_;
    Buffer buff_;
    int level_;
    bool isAsync_;

    FILE* fp_;
    std::unique_ptr<BlockDeque<std::string>> deque_;
    std::unique_ptr<std::thread> writeThread_;
    std::mutex mtx_;

};  // endof class Log

#define LOG_BASE(level, format, ...) \
    do {\
        mfwu_webserver::Log* log = mfwu_webserver::Log::Instance(); \
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush(); \
        }\
    } while(0);

#define LOG_DEBUG(format, ...)do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...)do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...)do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...)do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

// inline void log_debug(const char* format, ...) {
//     va_list vaList;
//     va_start(vaList, format);
//     LOG_DEBUG(format, vaList);
//     va_end(vaList);
// }

}  // endof namespace mfwu_webserver


#endif  // __LOG_HPP__