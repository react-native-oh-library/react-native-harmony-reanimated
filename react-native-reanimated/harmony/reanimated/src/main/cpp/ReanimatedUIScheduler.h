#pragma once
#include <functional>
#include <UIScheduler.h>
#include <memory>
#include "RNOH/TaskExecutor/TaskExecutor.h"

namespace rnoh {

    class ReanimatedUIScheduler : public reanimated::UIScheduler {
    private:
        std::shared_ptr<TaskExecutor> m_taskExecutor;

    public:
        ReanimatedUIScheduler(std::shared_ptr<TaskExecutor> taskExecutor);
        void scheduleOnUI(std::function<void()> job) override;
    };
} // namespace rnoh