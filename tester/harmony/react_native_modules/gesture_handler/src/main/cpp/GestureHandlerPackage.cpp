#include "GestureHandlerPackage.h"
#include "RNGestureHandlerRootViewComponentDescriptor.h"
#include "RNGestureHandlerButtonComponentDescriptor.h"
#include "RNGestureHandlerModule.h"

using namespace rnoh;
using namespace facebook;

class GestureHandlerTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "RNGestureHandlerModule") {
            return std::make_shared<RNGestureHandlerModule>(ctx, name);
        }
        return nullptr;
    };
};

std::unique_ptr<TurboModuleFactoryDelegate> GestureHandlerPackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<GestureHandlerTurboModuleFactoryDelegate>();
}

std::vector<react::ComponentDescriptorProvider> GestureHandlerPackage::createComponentDescriptorProviders() {
    return {
      react::concreteComponentDescriptorProvider<react::RNGestureHandlerRootViewComponentDescriptor>(),
      react::concreteComponentDescriptorProvider<react::RNGestureHandlerButtonComponentDescriptor>(),
    };
}

EventEmitRequestHandlers GestureHandlerPackage::createEventEmitRequestHandlers() {
    return {
        std::make_shared<react::RNGestureHandlerRootViewEventEmitRequestHandler>(),
    };
}