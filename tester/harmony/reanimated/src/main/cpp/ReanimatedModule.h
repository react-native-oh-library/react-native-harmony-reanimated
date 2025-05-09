#pragma once

#include "RNOH/ArkTSTurboModule.h"
#include "NativeReanimatedModule.h"
#include "ReanimatedNodesManager.h"

namespace rnoh {

class JSI_EXPORT ReanimatedModule : public std::enable_shared_from_this<ReanimatedModule>, public ArkTSTurboModule {
public:
    ReanimatedModule(const ArkTSTurboModule::Context ctx, const std::string name);
    ~ReanimatedModule() override;
    void installTurboModule(facebook::jsi::Runtime &rt);
    void callKeyBord(double height, int status);
private:
    std::weak_ptr<reanimated::NativeReanimatedModule> weakNativeReanimatedModule_;
    void injectDependencies(facebook::jsi::Runtime &rt);
    std::shared_ptr<facebook::react::EventListener> eventListener;
    std::function<void(int keyboardState, int height)> keyboardEventDataUpdater_;
};

} // namespace rnoh