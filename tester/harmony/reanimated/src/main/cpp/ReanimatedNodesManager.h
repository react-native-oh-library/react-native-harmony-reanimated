#pragma once
#include <vector>
#include <functional>
#include <native_vsync/native_vsync.h>
#include "RNOH/NativeVsyncHandle.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"

using ReanimatedPerformOperations = std::function<void()>;
using ReanimatedOnAnimationCallback = std::function<void(double)>;
using ReanimatedEventHandler = std::function<void()>;
namespace rnoh {
class ReanimatedNodesManager : public std::enable_shared_from_this<ReanimatedNodesManager> {
    ReanimatedPerformOperations performOperations;
    std::vector<ReanimatedOnAnimationCallback> onAnimationCallbacks;
    NativeVsyncHandle m_vsyncHandle;
    std::function<void(TaskExecutor::Task &&)> runOnTheMainThread;

public:
    ReanimatedNodesManager(std::function<void(TaskExecutor::Task &&)> runOnTheMainThread);
    ~ReanimatedNodesManager() noexcept = default;
    void maybeFlushUIUpdatesQueue();
    void registerPerformOperations(ReanimatedPerformOperations reanimatedPerformOperations);
    void postOnAnimation(ReanimatedOnAnimationCallback callback);
    void operationBatchDidComplete();
    void onAnimationFrame(int64_t timestamp);
};
} // namespace rnoh