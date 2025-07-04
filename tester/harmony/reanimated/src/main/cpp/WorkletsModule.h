#pragma once

#include "RNOH/ArkTSTurboModule.h"
#include "worklets/NativeModules/WorkletsModuleProxy.h"

namespace worklets {

class JSI_EXPORT WorkletsModule : public std::enable_shared_from_this<WorkletsModule>, public rnoh::ArkTSTurboModule {
public:
    WorkletsModule(const ArkTSTurboModule::Context ctx, const std::string name);
    ~WorkletsModule() override;
    void installTurboModule(facebook::jsi::Runtime &rt, const std::string valueUnpackerCode);
    std::shared_ptr<worklets::WorkletsModuleProxy> getWorkletsModuleProxy();
private:
    std::shared_ptr<worklets::WorkletsModuleProxy> workletsModuleProxy_ = nullptr;
};

} // namespace worklets