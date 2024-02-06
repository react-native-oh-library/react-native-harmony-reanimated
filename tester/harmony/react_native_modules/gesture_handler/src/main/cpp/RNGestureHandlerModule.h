#pragma once

#include "RNOH/ArkTSTurboModule.h"

namespace rnoh {

class JSI_EXPORT RNGestureHandlerModule : public ArkTSTurboModule {
public:
    RNGestureHandlerModule(const ArkTSTurboModule::Context ctx, const std::string name);
};

} // namespace rnoh