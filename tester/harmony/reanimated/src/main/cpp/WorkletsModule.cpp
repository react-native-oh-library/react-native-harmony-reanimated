#include "RNOH/ArkTSTurboModule.h"
#include "ReanimatedUIScheduler.h"
#include "WorkletsModule.h"
#include "RNRuntimeWorkletDecorator.h"
#include "PlatformLogger.h"

using namespace facebook;
namespace worklets {
jsi::Value installTurboModule(facebook::jsi::Runtime &rt, react::TurboModule &turboModule,
    const facebook::jsi::Value *args, size_t count)
{
    auto self = static_cast<WorkletsModule *>(&turboModule);
    if (count < 1) {
        LOG(ERROR) << "WorkletsModule: Missing required argument";
        return facebook::jsi::Value(false);
    }
    if (!args[0].isString()) {
        LOG(ERROR) << "WorkletsModule: First argument must be a string";
        return facebook::jsi::Value(false);
    }

    std::string valueUnpackerCode = args[0].getString(rt).utf8(rt);
    self->installTurboModule(rt, valueUnpackerCode);
    return facebook::jsi::Value(true);
}

WorkletsModule::WorkletsModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name)
{
    methodMap_ = { { "installTurboModule", { 0, worklets::installTurboModule } } };
}

WorkletsModule::~WorkletsModule()
{
    LOG(INFO) << "WorkletsModule::~WorkletsModule";
}

void WorkletsModule::installTurboModule(facebook::jsi::Runtime &runtime, std::string valueUnpackerCode)
{
    if (workletsModuleProxy_) {
        LOG(WARNING) << "WorkletsModuleProxy already initialized";
        return;
    }

    workletsModuleProxy_ = std::make_shared<WorkletsModuleProxy>(valueUnpackerCode, m_ctx.jsQueue, jsInvoker_,
        std::make_shared<worklets::JSScheduler>(runtime, jsInvoker_),
        std::make_shared<rnoh::ReanimatedUIScheduler>(m_ctx.taskExecutor));
    RNRuntimeWorkletDecorator::decorate(runtime, workletsModuleProxy_);
}
std::shared_ptr<worklets::WorkletsModuleProxy> WorkletsModule::getWorkletsModuleProxy()
{
    return workletsModuleProxy_;
}
} // namespace worklets