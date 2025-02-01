#define __UNIT_TEST_HPP__

#include "logger.hpp"

namespace mfwu {

class unit_test_logger {
public:
    int use_mfwu_logger();
};  // endof class unit_test_logger

int unit_test_logger::use_mfwu_logger() {
    mfwu::Logger logger = Logger();
    logger.addAppender(0, LogLevel::INFO);
    logger.addAppender(1, LogLevel::WARN, "logger.out");

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

}  // endof namespace mfwu