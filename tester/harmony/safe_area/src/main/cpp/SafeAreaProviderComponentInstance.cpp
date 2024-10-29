/**
 * MIT License
 *
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sys/param.h>
#include "SafeAreaProviderComponentInstance.h"
#include "TurboModuleRequest.h"

namespace rnoh {

    SafeAreaProviderComponentInstance::SafeAreaProviderComponentInstance(Context context)
        : CppComponentInstance(std::move(context)) {}

    void SafeAreaProviderComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                            std::size_t index) {
        CppComponentInstance::onChildInserted(childComponentInstance, index);
        m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    }

    void SafeAreaProviderComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
        CppComponentInstance::onChildRemoved(childComponentInstance);
        m_stackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
    };

    SafeAreaStackNode &SafeAreaProviderComponentInstance::getLocalRootArkUINode() { return m_stackNode; }

    void SafeAreaProviderComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
        CppComponentInstance::onPropsChanged(props);
    }

    void SafeAreaProviderComponentInstance::onEventEmitterChanged(SharedConcreteEventEmitter const &eventEmitter) {
        auto newEventEmitter = std::dynamic_pointer_cast<const ConcreteEventEmitter>(eventEmitter);
        if (!newEventEmitter) {
            return;
        }
        TurboModuleRequest request;
        safeArea::Event data = request.getTurboModuleData(this->m_deps);
        double x = getLayoutMetrics().frame.origin.x;
        double y = getLayoutMetrics().frame.origin.y;
        double width = getLayoutMetrics().frame.size.width;
        double height = getLayoutMetrics().frame.size.height;
        double providerTop = MAX(data.insets.top - y, 0);
        double providerRight = MAX(MIN(x + width - data.frame.width, 0) + data.insets.right, 0);
        double providerBottom = MAX(MIN(y + height - data.frame.height, 0) + data.insets.bottom, 0);
        double providerLeft = MAX(data.insets.left - x, 0);
        facebook::react::RNCSafeAreaProviderEventEmitter::OnInsetsChangeInsets insets = {providerTop, providerRight,
                                                                                         providerBottom, providerLeft};
        facebook::react::RNCSafeAreaProviderEventEmitter::OnInsetsChangeFrame frame = {x, y, width, height};
        facebook::react::RNCSafeAreaProviderEventEmitter::OnInsetsChange insetsChange = {insets, frame};
        newEventEmitter->onInsetsChange(insetsChange);
    }

} // namespace rnoh
