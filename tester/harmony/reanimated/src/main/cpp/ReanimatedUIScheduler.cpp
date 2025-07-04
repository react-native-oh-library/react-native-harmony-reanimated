#include "ReanimatedUIScheduler.h"

namespace rnoh {
ReanimatedUIScheduler::ReanimatedUIScheduler(std::shared_ptr<TaskExecutor> taskExecutor) : m_taskExecutor(taskExecutor)
{}
void ReanimatedUIScheduler::scheduleOnUI(std::function<void()> job)
{
    if (m_taskExecutor->isOnTaskThread(TaskThread::MAIN)) {
        job();
        return;
    }

    m_taskExecutor->runTask(TaskThread::MAIN, [weakSelf = weak_from_this(), job = std::move(job)]() {
        if (auto self = weakSelf.lock()) {
            job();
        }
    });
}
} // namespace rnoh
