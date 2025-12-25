#pragma once
#ifdef RCT_NEW_ARCH_ENABLED

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/uimanager/UIManager.h>

#include <memory>
#include <set>

using namespace facebook;
using namespace react;

namespace reanimated {

using PropsMap = std::unordered_map<const facebook::react::ShadowNodeFamily*, folly::dynamic>;
using ChildrenMap =
    std::unordered_map<const ShadowNodeFamily *, std::unordered_set<int>>;

ShadowNode::Unshared cloneShadowTreeWithNewProps(
    const ShadowNode::Shared &oldRootNode,
    const ShadowNodeFamily &family,
    RawProps &&rawProps);

RootShadowNode::Unshared cloneShadowTreeWithNewPropsUnmounted(
    RootShadowNode::Unshared const &oldRootShadowNode,
    const PropsMap &propsMap);

} // namespace reanimated

#endif // RCT_NEW_ARCH_ENABLED
