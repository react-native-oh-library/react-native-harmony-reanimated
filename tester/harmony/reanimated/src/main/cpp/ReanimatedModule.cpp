#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/RNInstance.h"
#include "ReanimatedModule.h"
#include "ReanimatedUIScheduler.h"
#include "PlatformDepMethodsHolder.h"
#include "WorkletRuntimeCollector.h"
#include "RNRuntimeDecorator.h"
#include "TransformParser.h"
#include "RNOH/RNInstanceCAPI.h"

using namespace facebook;
using namespace reanimated;
namespace rnoh {

static double getMillisSinceEpoch() {
    auto now = std::chrono::high_resolution_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return frameTime.count();
}

jsi::Value installTurboModule(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                              const facebook::jsi::Value *args, size_t count) {
    auto self = static_cast<ReanimatedModule *>(&turboModule);
    self->installTurboModule(rt);
    return facebook::jsi::Value::undefined();
}

ReanimatedModule::ReanimatedModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_ = {{"installTurboModule", {0, rnoh::installTurboModule}}};
}

ReanimatedModule::~ReanimatedModule() {
    LOG(INFO) << "ReanimatedModule::~ReanimatedModule";
    if (eventListener) {
        m_ctx.scheduler->removeEventListener(eventListener);
    }
    if (keyboardEventDataUpdater_) {
        keyboardEventDataUpdater_ = nullptr;
    }
}

void ReanimatedModule::installTurboModule(facebook::jsi::Runtime &rt) {
    auto nodesManager = std::make_shared<ReanimatedNodesManager>(
        [weakExecutor = std::weak_ptr(m_ctx.taskExecutor)](TaskExecutor::Task &&task) {
            if (auto taskExecutor = weakExecutor.lock()) {
                taskExecutor->runTask(TaskThread::MAIN, std::move(task));
            }
        });

    std::shared_ptr<UIScheduler> uiScheduler = std::make_shared<ReanimatedUIScheduler>(m_ctx.taskExecutor);
    auto maybeFlushUIUpdatesQueueFunction = [nodesManager]() { nodesManager->maybeFlushUIUpdatesQueue(); };
    auto requestRender = [weakSelf = weak_from_this(), nodesManager](std::function<void(double)> onRender,
                                                                     jsi::Runtime & /*rt*/) {
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

    auto synchronouslyUpdateUIPropsFunction = [weakInstance = m_ctx.instance](jsi::Runtime &rt, Tag tag,
                                                                              const jsi::Object &props) {
        auto dynamic = jsi::dynamicFromValue(rt, jsi::Value(rt, props));
        auto instance = weakInstance.lock();
        auto instanceCapi = std::dynamic_pointer_cast<RNInstanceCAPI>(instance);
        if (!instanceCapi) {
            return;
        }
        auto componentInstance = instanceCapi->findComponentInstanceByTag(tag);
        if (!componentInstance) {
            return;
        }

        // We want to restore the ignored props after the update,
        // because reanimated handles setting correct props on React renders on its own
        auto ignoredProps = componentInstance->getIgnoredPropKeys();
        instanceCapi->synchronouslyUpdateViewOnUIThread(tag, dynamic);
        componentInstance->setIgnoredPropKeys(std::move(ignoredProps));
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
                                      bool isStatusBarTranslucent) {
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
        napiTurboModuleObject.call("setGestureHandlerState", {napiTag, napiState});
    };

    PlatformDepMethodsHolder platformDepMethodsHolder = {
        requestRender,
        synchronouslyUpdateUIPropsFunction,
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

    auto nativeReanimatedModule =
        std::make_shared<NativeReanimatedModule>(rt, jsInvoker_, m_ctx.jsQueue, uiScheduler, platformDepMethodsHolder);

    weakNativeReanimatedModule_ = nativeReanimatedModule;
    ReanimatedPerformOperations reanimatedPerformOperations = [weakNativeReanimatedModule =
                                                                   weakNativeReanimatedModule_]() {
        if (auto nativeReanimatedModule = weakNativeReanimatedModule.lock()) {
            nativeReanimatedModule->performOperations();
        }
    };
    nodesManager->registerPerformOperations(reanimatedPerformOperations);

    WorkletRuntimeCollector::install(rt);

    auto isReducedMotion = false;

    RNRuntimeDecorator::decorate(rt, nativeReanimatedModule, isReducedMotion);
    injectDependencies(rt);

    eventListener = std::make_shared<facebook::react::EventListener>(
        [weakNativeReanimatedModule = weakNativeReanimatedModule_,
         weakTaskExecutor = std::weak_ptr{m_ctx.taskExecutor}](facebook::react::RawEvent const &rawEvent) {
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
void ReanimatedModule::injectDependencies(facebook::jsi::Runtime & /*rt*/) {
    const auto uiManager = m_ctx.scheduler->getUIManager();
    if (auto nativeReanimatedModule = weakNativeReanimatedModule_.lock()) {
        nativeReanimatedModule->initializeFabric(uiManager);
    }
}

void ReanimatedModule::callKeyBord(double height, int status) {
    if (keyboardEventDataUpdater_) {
        keyboardEventDataUpdater_(status, height);
    }
}
} // namespace rnoh