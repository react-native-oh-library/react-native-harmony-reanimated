#include <PlatformLogger.h>
#include "glog/logging.h"

namespace worklets {
void PlatformLogger::log(const char *str)
{
    LOG(INFO) << str;
}

void PlatformLogger::log(const std::string &str)
{
    log(str.c_str());
}

void PlatformLogger::log(const double d)
{
    LOG(INFO) << d;
}

void PlatformLogger::log(const int i)
{
    LOG(INFO) << i;
}

void PlatformLogger::log(const bool b)
{
    LOG(INFO) << b;
}
} // namespace worklets
