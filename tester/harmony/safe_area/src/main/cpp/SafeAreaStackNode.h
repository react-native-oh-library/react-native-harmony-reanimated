#pragma once

#include "RNOH/arkui/ArkUINode.h"

namespace rnoh {
class SafeAreaStackNodeDelegate {
 public:
  virtual ~SafeAreaStackNodeDelegate() = default;
  virtual void onClick() {};
};

class SafeAreaStackNode : public ArkUINode {
 protected:
  SafeAreaStackNodeDelegate* m_stackNodeDelegate;

 public:
  SafeAreaStackNode();
  ~SafeAreaStackNode() override;

  void insertChild(ArkUINode& child, std::size_t index);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void onClick();
  void setStackNodeDelegate(SafeAreaStackNodeDelegate* safeAreaStackNodeDelegate);
  SafeAreaStackNode& setAlign(int32_t align);
};

} // namespace rnoh