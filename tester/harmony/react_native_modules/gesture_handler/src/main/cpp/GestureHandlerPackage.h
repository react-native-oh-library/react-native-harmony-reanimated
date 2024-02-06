#include "RNOH/Package.h"

namespace rnoh {

class GestureHandlerPackage : public Package {
  public:
    GestureHandlerPackage(Package::Context ctx) : Package(ctx) {}

    std::vector<facebook::react::ComponentDescriptorProvider> createComponentDescriptorProviders() override;

    EventEmitRequestHandlers createEventEmitRequestHandlers() override;

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;
};
} // namespace rnoh