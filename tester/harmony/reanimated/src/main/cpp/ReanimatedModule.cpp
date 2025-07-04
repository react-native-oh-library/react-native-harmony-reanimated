#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstance.h"
#include "ReanimatedModule.h"
#include "ReanimatedUIScheduler.h"
#include "PlatformDepMethodsHolder.h"
#include "WorkletRuntimeCollector.h"
#include "RNRuntimeDecorator.h"
#include "TransformParser.h"
#include "RNOH/RNInstanceCAPI.h"
#include "WorkletsModule.h"

using namespace facebook;
using namespace reanimated;
using namespace worklets;
namespace rnoh {
static double getMillisSinceEpoch()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return frameTime.count();
}

jsi::Value installTurboModule(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
    const facebook::jsi::Value *args, size_t count)
{
    auto self = static_cast<ReanimatedModule *>(&turboModule);
    self->installTurboModule(rt);
    return facebook::jsi::Value(true);
}

ReanimatedModule::ReanimatedModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name)
{
    methodMap_ = { { "installTurboModule", { 0, rnoh::installTurboModule } } };
}

ReanimatedModule::~ReanimatedModule()
{
    LOG(INFO) << "ReanimatedModule::~ReanimatedModule";
    if (eventListener) {
        m_ctx.scheduler->removeEventListener(eventListener);
    }
    if (keyboardEventDataUpdater_) {
        keyboardEventDataUpdater_ = nullptr;
    }
}

void ReanimatedModule::installTurboModule(facebook::jsi::Runtime &rt)
{
    auto nodesManager = std::make_shared<ReanimatedNodesManager>(
        [weakExecutor = std::weak_ptr(m_ctx.taskExecutor)](TaskExecutor::Task &&task) {
            if (auto taskExecutor = weakExecutor.lock()) {
                taskExecutor->runTask(TaskThread::MAIN, std::move(task));
            }
        });

    auto maybeFlushUIUpdatesQueueFunction = [nodesManager]() { nodesManager->maybeFlushUIUpdatesQueue(); };
    auto requestRender = [weakSelf = weak_from_this(), nodesManager](std::function<void(double)> onRender) {
        auto self = weakSelf.lock();
        if (!self) {
            return;
        }

        nodesManager->postOnAnimation(
            [weakReanimatedModule = self->weakNativeReanimatedModule_, onRender = std::move(onRender)](auto frameTime) {
                if (auto reanimatedModule = weakReanimatedModule.lock()) {
                    onRender(frameTime);
                }
            });
    };

    auto progressLayoutAnimation = [=](jsi::Runtime &rt, int tag, const jsi::Object &newStyle,
        bool isSharedTransition) {
        // noop
    };

    auto endLayoutAnimation = [=](int tag, bool removeView) {
        // noop
    };

    auto getAnimationTimestamp = getMillisSinceEpoch;

    auto registerSensorFunction = [](int sensorType, int interval, int iosReferenceFrame,
        std::function<void(double[], int)> setter) -> int {
        // TODO
        return -1;
    };
    auto unregisterSensorFunction = [](int sensorId) {
        // TODO
    };
    auto subscribeForKeyboardEventsFunction =
        [weakSelf = weak_from_this()](std::function<void(int keyboardState, int height)> keyboardEventDataUpdater,
        bool isStatusBarTranslucent, bool isNavigationBarTranslucent) {
            auto self = weakSelf.lock();
            if (!self) {
                return 0;
            }
            self->keyboardEventDataUpdater_ = std::move(keyboardEventDataUpdater);
            auto weakExecutor = std::weak_ptr(self->m_ctx.taskExecutor);
            if (auto taskExecutor = weakExecutor.lock()) {
                taskExecutor->runTask(TaskThread::MAIN, [ctx = self->m_ctx, isStatusBarTranslucent]() {
                    ArkJS arkJs(ctx.env);
                    // arkJs.createBoolean(isStatusBarTranslucent);
                    auto napiTurboModuleObject = arkJs.getObject(ctx.arkTSTurboModuleInstanceRef);
                    napiTurboModuleObject.call("subscribeKeyBordListeners", {});
                });
            }
            return 0;
        };
    auto unsubscribeFromKeyboardEventsFunction = [weakSelf = weak_from_this()](int listenerId) {
        auto self = weakSelf.lock();
        if (!self) {
            return 0;
        }
        auto weakExecutor = std::weak_ptr(self->m_ctx.taskExecutor);
        if (auto taskExecutor = weakExecutor.lock()) {
            taskExecutor->runTask(TaskThread::MAIN, [ctx = self->m_ctx]() {
                ArkJS arkJs(ctx.env);
                auto napiTurboModuleObject = arkJs.getObject(ctx.arkTSTurboModuleInstanceRef);
                napiTurboModuleObject.call("unsubscribeKeyBordListeners", {});
            });
        }
        return 0;
    };
    auto setGestureStateFunction = [weakSelf = weak_from_this()](int handlerTag, int newState) {
        auto self = weakSelf.lock();
        if (!self) {
            return;
        }
        ArkJS arkJs(self->m_ctx.env);
        auto napiTag = arkJs.createInt(handlerTag);
        auto napiState = arkJs.createInt(newState);
        auto napiTurboModuleObject = arkJs.getObject(self->m_ctx.arkTSTurboModuleInstanceRef);
        napiTurboModuleObject.call("setGestureHandlerState", { napiTag, napiState });
    };
    PlatformDepMethodsHolder platformDepMethodsHolder = {
        requestRender,
        getAnimationTimestamp,
        progressLayoutAnimation,
        endLayoutAnimation,
        registerSensorFunction,
        unregisterSensorFunction,
        setGestureStateFunction,
        subscribeForKeyboardEventsFunction,
        unsubscribeFromKeyboardEventsFunction,
        maybeFlushUIUpdatesQueueFunction,
    };
    auto isReducedMotion = false;
    auto isBridgeless = true;
    auto workletsModule = [weakSelf = weak_from_this()]() -> std::shared_ptr<WorkletsModule> {
        auto self = weakSelf.lock();
        if (!self) {
            return nullptr;
        }
        auto instance = self->m_ctx.instance.lock();
        if (instance == nullptr) {
            return nullptr;
        }
        return instance->getTurboModule<WorkletsModule>("WorkletsModule");
    };
    auto nativeReanimatedModule = std::make_shared<ReanimatedModuleProxy>(workletsModule()->getWorkletsModuleProxy(),
        rt, jsInvoker_, platformDepMethodsHolder, isBridgeless, isReducedMotion);
    weakNativeReanimatedModule_ = nativeReanimatedModule;
    nativeReanimatedModule->init(platformDepMethodsHolder);
    ReanimatedPerformOperations reanimatedPerformOperations = [weakNativeReanimatedModule =
                                                                   weakNativeReanimatedModule_]() {
        if (auto nativeReanimatedModule = weakNativeReanimatedModule.lock()) {
            nativeReanimatedModule->performOperations();
        }
    };
    nodesManager->registerPerformOperations(reanimatedPerformOperations);

    WorkletRuntimeCollector::install(rt);

    RNRuntimeDecorator::decorate(rt, nativeReanimatedModule);
    injectDependencies(rt);

    eventListener =
        std::make_shared<facebook::react::EventListener>([weakNativeReanimatedModule = weakNativeReanimatedModule_,
        weakTaskExecutor = std::weak_ptr{ m_ctx.taskExecutor }](facebook::react::RawEvent const & rawEvent) {
            auto taskExecutor = weakTaskExecutor.lock();
            auto nativeReanimatedModule = weakNativeReanimatedModule.lock();
            if (!nativeReanimatedModule || !taskExecutor || !taskExecutor->isOnTaskThread(TaskThread::MAIN)) {
                return false;
            }

            auto eventType = rawEvent.type;
            auto frameTime = getMillisSinceEpoch();

            // React Native prefixes event names with "top". If the event name starts with "on", the result is that the
            // event on the native side is prefixed with "topOn", while the JS side expects the event name to be without
            // the "on" prefix.
            if (eventType.rfind("topOn", 0) == 0) {
                auto eventCopy = rawEvent;
                eventCopy.type = "on" + eventType.substr(5);
                return nativeReanimatedModule->handleRawEvent(eventCopy, frameTime);
            } else if (eventType.rfind("topLayout", 0) == 0) { // 针对onLayout事件，不阻断，采用系统处理
                return false;
            }
            return nativeReanimatedModule->handleRawEvent(rawEvent, frameTime);
        });
    m_ctx.scheduler->addEventListener(eventListener);
}
void ReanimatedModule::injectDependencies(facebook::jsi::Runtime & /* rt */)
{
    const auto uiManager = m_ctx.scheduler->getUIManager();
    if (auto nativeReanimatedModule = weakNativeReanimatedModule_.lock()) {
        nativeReanimatedModule->initializeFabric(uiManager);
    }
}

void ReanimatedModule::callKeyBord(double height, int status)
{
    if (keyboardEventDataUpdater_) {
        keyboardEventDataUpdater_(status, height);
    }
}
} // namespace rnoh