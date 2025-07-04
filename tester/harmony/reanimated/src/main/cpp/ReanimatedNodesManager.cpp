#include "ReanimatedNodesManager.h"

constexpr int64_t NANO_TO_MILLIS = 1000000;

namespace rnoh {
static void vsyncHandleCallback(long long timestamp, void *data)
{
    auto self = static_cast<ReanimatedNodesManager *>(data);
    self->onAnimationFrame(timestamp);
}
ReanimatedNodesManager::ReanimatedNodesManager(std::function<void(TaskExecutor::Task &&)> runOnTheMainThread)
    : m_vsyncHandle("ReanimatedNodesManager"), runOnTheMainThread(std::move(runOnTheMainThread))
{}

void ReanimatedNodesManager::maybeFlushUIUpdatesQueue()
{
    performOperations();
};

void ReanimatedNodesManager::registerPerformOperations(ReanimatedPerformOperations reanimatedPerformOperations)
{
    performOperations = std::move(reanimatedPerformOperations);
}

void ReanimatedNodesManager::postOnAnimation(ReanimatedOnAnimationCallback callback)
{
    onAnimationCallbacks.push_back(std::move(callback));
    m_vsyncHandle.requestFrame(vsyncHandleCallback, this);
}

void ReanimatedNodesManager::operationBatchDidComplete()
{
    performOperations();
}

void ReanimatedNodesManager::onAnimationFrame(int64_t timestamp)
{
    runOnTheMainThread([weakSelf = weak_from_this(), timestamp]() {
        auto self = weakSelf.lock();
        if (!self) {
            return;
        }
        std::vector<ReanimatedOnAnimationCallback> callbacks;
        std::swap(callbacks, self->onAnimationCallbacks);

        for (auto &callback : callbacks) {
            callback(timestamp / NANO_TO_MILLIS);
        }
        self->performOperations();
    });
}
} // namespace rnoh