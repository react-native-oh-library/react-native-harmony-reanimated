#include "RNOH/ArkTSTurboModule.h"
#include "ReanimatedModule.h"
#include "ReanimatedUIScheduler.h"
#include "PlatformDepMethodsHolder.h"
#include "WorkletRuntimeCollector.h"
#include "RNRuntimeDecorator.h"
#include "TransformParser.h"

using namespace facebook;
using namespace reanimated;
namespace rnoh {
    jsi::Value installTurboModule(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
                                  const facebook::jsi::Value *args, size_t count) {
        auto self = static_cast<ReanimatedModule *>(&turboModule);
        self->installTurboModule(rt);
        return facebook::jsi::Value::undefined();
    }

    ReanimatedModule::ReanimatedModule(const ArkTSTurboModule::Context ctx, const std::string name)
        : ArkTSTurboModule(ctx, name), nodesManager([this](TaskExecutor::Task &&task) {
              m_ctx.taskExecutor->runTask(TaskThread::MAIN, std::move(task));
          }) {
        methodMap_ = {{"installTurboModule", {0, rnoh::installTurboModule}}};
    }

    void ReanimatedModule::installTurboModule(facebook::jsi::Runtime &rt) {
        auto jsQueue = m_ctx.jsQueue;
        auto jsInvoker = this->jsInvoker_;

        std::shared_ptr<UIScheduler> uiScheduler = std::make_shared<ReanimatedUIScheduler>(m_ctx.taskExecutor);
        auto maybeFlushUIUpdatesQueueFunction = [this]() { nodesManager.maybeFlushUIUpdatesQueue(); };
        auto requestRender = [this](std::function<void(double)> onRender, jsi::Runtime &rt) {
            ReanimatedOnAnimationCallback callback = [onRender = std::move(onRender)](double frameTimestamp) {
                onRender(frameTimestamp);
            };
            nodesManager.postOnAnimation(callback);
        };
        auto synchronouslyUpdateUIPropsFunction = [this](jsi::Runtime &rt, Tag tag, const jsi::Object &props) {
            auto dynamic = jsi::dynamicFromValue(rt, jsi::Value(rt, props));
            auto dynamicProps = dynamic.items();
            bool shouldEraseTransform = false;
            for (auto &prop : dynamicProps) {
                if (prop.first.getString() == "transform") {
                    if (prop.second.isArray()) {
                        prop.second = parseTransform(prop.second);
                    } else {
                        shouldEraseTransform = true;
                    }
                }
            }
            if (shouldEraseTransform) {
                dynamic.erase("transform");
            }
            ArkJS arkJs(m_ctx.env);
            auto napiArgs = arkJs.convertIntermediaryValueToNapiValue(dynamic);
            auto napiTag = arkJs.createInt(tag);
            auto napiTurboModuleObject = arkJs.getObject(m_ctx.arkTsTurboModuleInstanceRef);
            napiTurboModuleObject.call("setViewProps", {napiTag, napiArgs});
        };
        auto progressLayoutAnimation = [=](jsi::Runtime &rt, int tag, const jsi::Object &newStyle,
                                           bool isSharedTransition) {
            // noop
        };

        auto endLayoutAnimation = [=](int tag, bool removeView) {
            // noop
        };
        auto getAnimationTimestamp = []() {
            auto now = std::chrono::high_resolution_clock::now();
            auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            return frameTime.count();
        };

        auto registerSensorFunction = [](int sensorType, int interval, int iosReferenceFrame,
                                         std::function<void(double[], int)> setter) -> int {
            // TODO
            return -1;
        };
        auto unregisterSensorFunction = [](int sensorId) {
            // TODO
        };
        auto subscribeForKeyboardEventsFunction =
            [](std::function<void(int keyboardState, int height)> keyboardEventDataUpdater,
               bool isStatusBarTranslucent) {
                // TODO
                return 0;
            };
        auto unsubscribeFromKeyboardEventsFunction = [](int listenerId) {
            // TODO
        };
        auto setGestureStateFunction = [](int handlerTag, int newState) {
            // TODO
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
            std::make_shared<NativeReanimatedModule>(rt, jsInvoker, jsQueue, uiScheduler, platformDepMethodsHolder);

        weakNativeReanimatedModule_ = nativeReanimatedModule;
        ReanimatedPerformOperations reanimatedPerformOperations = [this]() {
            if (auto nativeReanimatedModule = weakNativeReanimatedModule_.lock()) {
                nativeReanimatedModule->performOperations();
            }
        };
        nodesManager.registerPerformOperations(reanimatedPerformOperations);

        WorkletRuntimeCollector::install(rt);

        auto isReducedMotion = false;

        RNRuntimeDecorator::decorate(rt, nativeReanimatedModule, isReducedMotion);
        injectDependencies(rt);
    }
    void ReanimatedModule::injectDependencies(facebook::jsi::Runtime &rt) {
        const auto uiManager = m_ctx.scheduler->getUIManager();
        if (auto nativeReanimatedModule = weakNativeReanimatedModule_.lock()) {
            nativeReanimatedModule->initializeFabric(uiManager);
        }
    }
} // namespace rnoh