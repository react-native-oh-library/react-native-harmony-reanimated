#include "ReanimatedNodesManager.h"
#include "glog/logging.h"

constexpr int64_t NANO_TO_MILLIS = 1000000;

namespace rnoh {
    static void vsyncHandleCallback(long long timestamp, void *data) {
        auto self = static_cast<ReanimatedNodesManager *>(data);
        self->onAnimationFrame(timestamp);
    }
    ReanimatedNodesManager::ReanimatedNodesManager(std::function<void(TaskExecutor::Task &&)> runOnTheMainThread)
        : m_vsyncHandle("ReanimatedTurboModule"), runOnTheMainThread(runOnTheMainThread){};
    void ReanimatedNodesManager::maybeFlushUIUpdatesQueue() { performOperations(); };
    void ReanimatedNodesManager::registerPerformOperations(ReanimatedPerformOperations &reanimatedPerformOperations) {
        performOperations = reanimatedPerformOperations;
    }
    void ReanimatedNodesManager::postOnAnimation(ReanimatedOnAnimationCallback &callback) {
        onAnimationCallbacks.push_back(callback);
        m_vsyncHandle.requestFrame(vsyncHandleCallback, this);
    }
    void ReanimatedNodesManager::operationBatchDidComplete() { performOperations(); }
    void ReanimatedNodesManager::onAnimationFrame(int64_t timestamp) {
        runOnTheMainThread([this, timestamp]() {
            std::vector<ReanimatedOnAnimationCallback> callbacks = this->onAnimationCallbacks;
            this->onAnimationCallbacks.clear();

            for (auto &callback : callbacks) {
                callback(timestamp / NANO_TO_MILLIS);
            }
            this->performOperations();
        });
    }

} // namespace rnoh