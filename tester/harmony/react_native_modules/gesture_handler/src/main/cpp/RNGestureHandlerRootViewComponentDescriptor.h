#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace facebook {
namespace react {

extern const char RNGestureHandlerRootViewComponentName[] = "RNGestureHandlerRootView";

class RNGestureHandlerRootViewEventEmitter : public ViewEventEmitter {
    using ViewEventEmitter::ViewEventEmitter;

  public:
    void emit(std::string type, const folly::dynamic &payload) const {
        this->dispatchEvent(type, payload);
    }
};

class RNGestureHandlerRootViewEventEmitRequestHandler : public rnoh::EventEmitRequestHandler {
    void handleEvent(rnoh::EventEmitRequestHandler::Context const &ctx) override {
        std::string prefix = "RNGestureHandlerRootView";
        size_t prefixPos = ctx.eventName.find(prefix);
        if (prefixPos != std::string::npos) {
            ArkJS arkJs(ctx.env);
            auto eventEmitter = ctx.shadowViewRegistry->getEventEmitter<RNGestureHandlerRootViewEventEmitter>(ctx.tag);
            if (eventEmitter != nullptr) {
                auto payload = arkJs.getDynamic(ctx.payload);
                std::string eventName = ctx.eventName.substr(prefixPos + prefix.length());
                eventEmitter->emit(eventName, payload);
            }
        }
    }
};

class RNGestureHandlerRootViewProps : public ViewProps {
  public:
    RNGestureHandlerRootViewProps() = default;

    RNGestureHandlerRootViewProps(const PropsParserContext &context, const RNGestureHandlerRootViewProps &sourceProps, const RawProps &rawProps)
        : ViewProps(context, sourceProps, rawProps) {}
};

using RNGestureHandlerRootViewShadowNode = ConcreteViewShadowNode<
    RNGestureHandlerRootViewComponentName,
    RNGestureHandlerRootViewProps,
    RNGestureHandlerRootViewEventEmitter>;

class RNGestureHandlerRootViewComponentDescriptor final
    : public ConcreteComponentDescriptor<RNGestureHandlerRootViewShadowNode> {
  public:
    RNGestureHandlerRootViewComponentDescriptor(ComponentDescriptorParameters const &parameters)
        : ConcreteComponentDescriptor(parameters) {}
};

} // namespace react
} // namespace facebook
