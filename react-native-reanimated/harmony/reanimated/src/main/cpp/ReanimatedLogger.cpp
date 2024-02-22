#include "ReanimatedLogger.h"
#include "glog/logging.h"

namespace reanimated {

    std::unique_ptr<reanimated::LoggerInterface> Logger::instance = std::make_unique<ReanimatedLogger>();

    void ReanimatedLogger::log(const char *str) { LOG(INFO) << str; }

    void ReanimatedLogger::log(const std::string &str) { log(str.c_str()); }

    void ReanimatedLogger::log(double d) { LOG(INFO) << d; }

    void ReanimatedLogger::log(int i) { LOG(INFO) << i; }

    void ReanimatedLogger::log(bool b) { LOG(INFO) << b; }

} // namespace reanimated
