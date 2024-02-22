#include <ReanimatedHiddenHeaders.h>
#include <stdio.h>

namespace reanimated {

    class ReanimatedLogger : public LoggerInterface {
    public:
        void log(const char *str) override;
        void log(const std::string &str) override;
        void log(double d) override;
        void log(int i) override;
        void log(bool b) override;
    };

} // namespace reanimated