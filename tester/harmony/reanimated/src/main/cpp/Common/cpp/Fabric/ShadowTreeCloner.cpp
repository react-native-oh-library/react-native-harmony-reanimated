#ifdef RCT_NEW_ARCH_ENABLED

#include <ShadowTreeCloner.h>
#include <react/renderer/core/DynamicPropsUtilities.h>

#include "ShadowTreeCloner.h"
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace reanimated {

ChildrenMap calculateChildrenMap(const RootShadowNode &oldRootNode, const PropsMap &propsMap) {
    ChildrenMap childrenMap;
    for (auto &[family, _] : propsMap) {
        const auto ancestors = family->getAncestors(oldRootNode);

        for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) {
            const auto &[parentNode, index] = *it;
            const auto parentFamily = &parentNode.get().getFamily();
            auto &affectedChildren = childrenMap[parentFamily];

            int intIndex = static_cast<int>(index);
            if (affectedChildren.find(intIndex) != affectedChildren.end()) {
                continue;
            }

            affectedChildren.insert(intIndex);
        }
    }
    return childrenMap;
}

ShadowNode::Unshared cloneShadowTreeWithNewPropsRecursive(const ShadowNode &shadowNode, const ChildrenMap &childrenMap,
                                                          const PropsMap &propsMap) {
    const auto &familyRef = shadowNode.getFamily(); // 获取Family的const引用
    const auto affectedChildrenIt = childrenMap.find(&familyRef);

    PropsMap::const_iterator propsIt;

    propsIt = propsMap.end();
    for (auto it = propsMap.cbegin(); it != propsMap.cend(); ++it) {
        if (it->first == &familyRef) {
            propsIt = it;
            break;
        }
    }

    auto children = shadowNode.getChildren();

    if (affectedChildrenIt != childrenMap.end()) {
        for (const auto index : affectedChildrenIt->second) {
            children[index] = cloneShadowTreeWithNewPropsRecursive(*children[index], childrenMap, propsMap);
        }
    }

    Props::Shared newProps = nullptr;

    if (propsIt != propsMap.end()) {
        PropsParserContext propsParserContext{shadowNode.getSurfaceId(), *shadowNode.getContextContainer()};
        newProps = shadowNode.getProps();

        const folly::dynamic &dynamicProps = propsIt->second;
        newProps = shadowNode.getComponentDescriptor().cloneProps(propsParserContext, newProps, RawProps(dynamicProps));
    }

    const auto result = shadowNode.clone({newProps ? newProps : ShadowNodeFragment::propsPlaceholder(),
                                          std::make_shared<ShadowNode::ListOfShared>(children), shadowNode.getState()});

    return result;
}


ShadowNode::Unshared cloneShadowTreeWithNewPropsUnmountedRecursive(ShadowNode::Shared const &oldShadowNode,
                                                                   const ChildrenMap &childrenMap,
                                                                   const PropsMap &propsMap) {
    auto shadowNode = std::const_pointer_cast<ShadowNode>(oldShadowNode);
    auto layoutableShadowNode = std::dynamic_pointer_cast<LayoutableShadowNode>(shadowNode);
    if (layoutableShadowNode) {
        layoutableShadowNode->dirtyLayout();
    }

    const auto &familyRef = shadowNode->getFamily();
    const auto affectedChildrenIt = childrenMap.find(&familyRef);
    PropsMap::const_iterator propsIt;

    propsIt = propsMap.end();
    for (auto it = propsMap.cbegin(); it != propsMap.cend(); ++it) {
        if (it->first == &familyRef) {
            propsIt = it;
            break;
        }
    }
    auto children = shadowNode->getChildren();

    if (affectedChildrenIt != childrenMap.end()) {
        for (const auto index : affectedChildrenIt->second) {
            auto clone = cloneShadowTreeWithNewPropsUnmountedRecursive(children[index], childrenMap, propsMap);
            if (clone != children[index]) {
                shadowNode->replaceChild(*children[index], clone, index);
            }
        }
    }

    Props::Shared newProps = nullptr;

    if (propsIt != propsMap.end()) {
        PropsParserContext propsParserContext{shadowNode->getSurfaceId(), *shadowNode->getContextContainer()};
        newProps = shadowNode->getProps();

        const folly::dynamic &dynamicProps = propsIt->second;
        newProps =
            shadowNode->getComponentDescriptor().cloneProps(propsParserContext, newProps, RawProps(dynamicProps));
    }
    
    if (newProps && !shadowNode->getSealed()) {
        auto &props = shadowNode->getProps();
        auto &mutableProps = const_cast<Props::Shared &>(props);
        mutableProps = newProps;
        auto layoutableShadowNode = static_pointer_cast<YogaLayoutableShadowNode>(shadowNode);
        layoutableShadowNode->updateYogaProps();
    }

    return shadowNode;
}

ShadowNode::Unshared cloneShadowTreeWithNewProps(
    const ShadowNode::Shared &oldRootNode,
    const ShadowNodeFamily &family,
    RawProps &&rawProps) {
  // adapted from ShadowNode::cloneTree

  auto ancestors = family.getAncestors(*oldRootNode);

  if (ancestors.empty()) {
    return ShadowNode::Unshared{nullptr};
  }

  auto &parent = ancestors.back();
  auto &source = parent.first.get().getChildren().at(parent.second);

  PropsParserContext propsParserContext{
      source->getSurfaceId(), *source->getContextContainer()};
  const auto props = source->getComponentDescriptor().cloneProps(
      propsParserContext, source->getProps(), std::move(rawProps));

  auto newChildNode = source->clone({/* .props = */ props, ShadowNodeFragment::childrenPlaceholder(), source->getState()});

  for (auto it = ancestors.rbegin(); it != ancestors.rend(); ++it) {
    auto &parentNode = it->first.get();
    auto childIndex = it->second;

    auto children = parentNode.getChildren();
    const auto &oldChildNode = *children.at(childIndex);
    react_native_assert(ShadowNode::sameFamily(oldChildNode, *newChildNode));

    if (!parentNode.getSealed()) {
      // Optimization: if a ShadowNode is unsealed, we can directly update its
      // children instead of cloning the whole path to the root node.
      auto &parentNodeNonConst = const_cast<ShadowNode &>(parentNode);
      parentNodeNonConst.replaceChild(oldChildNode, newChildNode, childIndex);
      // Unfortunately, `replaceChild` does not update Yoga nodes, so we need to
      // update them manually here.
      static_cast<YogaLayoutableShadowNode *>(&parentNodeNonConst)
          ->updateYogaChildren();
      return std::const_pointer_cast<ShadowNode>(oldRootNode);
    }

    children[childIndex] = newChildNode;

    newChildNode = parentNode.clone({
        ShadowNodeFragment::propsPlaceholder(),
        std::make_shared<ShadowNode::ListOfShared>(children),
        parentNode.getState()
    });
  }

  return std::const_pointer_cast<ShadowNode>(newChildNode);
}

RootShadowNode::Unshared cloneShadowTreeWithNewPropsUnmounted(
    RootShadowNode::Unshared const &oldRootNode,
    const PropsMap &propsMap) {
  auto childrenMap = calculateChildrenMap(*oldRootNode, propsMap);

  // This cast is safe, because this function returns a clone
  // of the oldRootNode, which is an instance of RootShadowNode
  return std::static_pointer_cast<RootShadowNode>(
      cloneShadowTreeWithNewPropsUnmountedRecursive(
          oldRootNode, childrenMap, propsMap));
}

} // namespace reanimated

#endif // RCT_NEW_ARCH_ENABLED
