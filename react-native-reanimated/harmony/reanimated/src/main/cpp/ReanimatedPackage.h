#include "RNOH/Package.h"

namespace rnoh {

    class ReanimatedPackage : public Package {
    public:
        ReanimatedPackage(Package::Context ctx) : Package(ctx) {}
    
        std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;
    };
} // namespace rnoh
