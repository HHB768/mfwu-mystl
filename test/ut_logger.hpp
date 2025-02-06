#define __UNIT_TEST_LOGGER__

#include "logger.hpp"
#include "webserver/log.hpp"

namespace mfwu {

class unit_test_logger {
public:
    int use_mfwu_logger();
    int use_webserver_logger();
};  // endof class unit_test_logger

int unit_test_logger::use_mfwu_logger() {
    std::cout << "\n------- Test: use mfwu logger -------\n";
    mfwu::Logger logger = Logger();
    logger.addAppender(0, LogLevel::INFO);
    logger.addAppender(1, LogLevel::WARN, "./log/logger.out");

    srand(time(nullptr));

    start_time = std::chrono::high_resolution_clock::now();
    LogEvent::ptr event = std::make_shared<LogEvent>("debug");
    logger.debug(event);
    event->refresh("--- INFO MSG ---");
    logger.info(event);
    event->refresh("--- WARN MSG ---");
    logger.warn(event);
    event->refresh("--- ERROR MSG ---");
    logger.log(LogLevel::ERROR, event);

    return 0; 
}

int unit_test_logger::use_webserver_logger() {
    std::cout << "\n------- Test: use webserver logger -------\n";
#ifdef __UNIT_TEST_LOGGER__
#ifndef __UNIT_TEST_LOGGER_BRIEF__
        mfwu_webserver::Log::Instance()->init(1, "./log", ".log", 1024);
        LOG_DEBUG("DEBUG MSG %d", 0);
        LOG_INFO("INFO MSG %d", 1);
        LOG_WARN("WARN MSG %d", 2);
        LOG_ERROR("ERROR MSG %d", 3);
#else  // !UNIT_TEST_LOGGER_BRIEF__
        // 我不到啊
#endif  // UNIT_TEST_LOGGER_BRIEF__
#endif  // UNIT_TEST_LOGGER__
    

    return 0;
}

}  // endof namespace mfwu