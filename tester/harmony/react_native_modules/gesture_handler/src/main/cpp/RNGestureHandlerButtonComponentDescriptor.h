#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace facebook {
namespace react {

extern const char RNGestureHandlerButtonComponentName[] = "RNGestureHandlerButton";

class RNGestureHandlerButtonEventEmitter : public ViewEventEmitter {
    using ViewEventEmitter::ViewEventEmitter;

  public:
    void emit(std::string type, const folly::dynamic &payload) const {
        this->dispatchEvent(type, payload);
    }
};

class RNGestureHandlerButtonEventEmitRequestHandler : public rnoh::EventEmitRequestHandler {
    void handleEvent(rnoh::EventEmitRequestHandler::Context const &ctx) override {
        std::string prefix = "RNGestureHandlerButton";
        size_t prefixPos = ctx.eventName.find(prefix);
        if (prefixPos != std::string::npos) {
            ArkJS arkJs(ctx.env);
            auto eventEmitter = ctx.shadowViewRegistry->getEventEmitter<RNGestureHandlerButtonEventEmitter>(ctx.tag);
            if (eventEmitter != nullptr) {
                auto payload = arkJs.getDynamic(ctx.payload);
                std::string eventName = ctx.eventName.substr(prefixPos + prefix.length());
                eventEmitter->emit(eventName, payload);
            }
        }
    }
};

class RNGestureHandlerButtonProps : public ViewProps {
  public:
    RNGestureHandlerButtonProps() = default;

    RNGestureHandlerButtonProps(const PropsParserContext &context, const RNGestureHandlerButtonProps &sourceProps, const RawProps &rawProps)
        : ViewProps(context, sourceProps, rawProps) {}
};

using RNGestureHandlerButtonShadowNode = ConcreteViewShadowNode<
    RNGestureHandlerButtonComponentName,
    RNGestureHandlerButtonProps,
    RNGestureHandlerButtonEventEmitter>;

class RNGestureHandlerButtonComponentDescriptor final
    : public ConcreteComponentDescriptor<RNGestureHandlerButtonShadowNode> {
  public:
    RNGestureHandlerButtonComponentDescriptor(ComponentDescriptorParameters const &parameters)
        : ConcreteComponentDescriptor(parameters) {}
};

} // namespace react
} // namespace facebook
