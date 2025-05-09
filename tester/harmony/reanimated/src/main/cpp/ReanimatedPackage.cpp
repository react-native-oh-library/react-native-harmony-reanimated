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

class ReanimatedArkTsMessage : public ArkTSMessageHandler {
    void handleArkTSMessage(const Context &ctx) override {
        auto rnInstance = ctx.rnInstance.lock();
        if (rnInstance) {
            if (ctx.messageName == "REANIMATED_KEY_BOARD_CHANGE") {
                auto keyboardHeight = ctx.messagePayload["keyboardHeight"].asDouble();
                auto status = ctx.messagePayload["status"].asInt();
                rnInstance->getTurboModule<ReanimatedModule>("ReanimatedModule")->callKeyBord(keyboardHeight, status);
            }
        }
    }
};

std::vector<ArkTSMessageHandler::Shared> ReanimatedPackage::createArkTSMessageHandlers() {
    return {std::make_shared<ReanimatedArkTsMessage>()};
}
