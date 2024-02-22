#pragma once

#include "RNOH/ArkTSTurboModule.h"
#include "NativeReanimatedModule.h"
#include "ReanimatedNodesManager.h"

namespace rnoh {

    class JSI_EXPORT ReanimatedModule : public ArkTSTurboModule {
    public:
        ReanimatedModule(const ArkTSTurboModule::Context ctx, const std::string name);
        void installTurboModule(facebook::jsi::Runtime &rt);

    private:
        std::weak_ptr<reanimated::NativeReanimatedModule> weakNativeReanimatedModule_;
        void injectDependencies(facebook::jsi::Runtime &rt);
        ReanimatedNodesManager nodesManager;
    };

} // namespace rnoh