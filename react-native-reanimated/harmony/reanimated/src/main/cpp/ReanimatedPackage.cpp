#include "ReanimatedPackage.h"
#include "ReanimatedModule.h"

using namespace rnoh;
using namespace facebook;

class ReanimatedTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "ReanimatedModule") {
            return std::make_shared<ReanimatedModule>(ctx, name);
        }
        return nullptr;
    };
};

std::unique_ptr<TurboModuleFactoryDelegate> ReanimatedPackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<ReanimatedTurboModuleFactoryDelegate>();
}
