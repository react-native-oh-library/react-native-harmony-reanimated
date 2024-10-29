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

#include "RNCSafeAreaProviderEventEmitters.h"

namespace facebook {
    namespace react {

        void RNCSafeAreaProviderEventEmitter::onInsetsChange(OnInsetsChange event) const
        {
            dispatchEvent("insetsChange", [event = std::move(event)](jsi::Runtime &runtime) {
                auto payload = jsi::Object(runtime);
                {
                    auto insets = jsi::Object(runtime);
                    insets.setProperty(runtime, "top", event.insets.top);
                    insets.setProperty(runtime, "right", event.insets.right);
                    insets.setProperty(runtime, "bottom", event.insets.bottom);
                    insets.setProperty(runtime, "left", event.insets.left);
                    payload.setProperty(runtime, "insets", insets);
                }
                {
                    auto frame = jsi::Object(runtime);
                    frame.setProperty(runtime, "x", event.frame.x);
                    frame.setProperty(runtime, "y", event.frame.y);
                    frame.setProperty(runtime, "width", event.frame.width);
                    frame.setProperty(runtime, "height", event.frame.height);
                    payload.setProperty(runtime, "frame", frame);
                }
                return payload;
            });
        }

        void RNCSafeAreaViewEventEmitter::onSafeAreaValueChange(OnInsetsChangeInsets event) const {
            dispatchEvent("safeAreaValueChange", [event = std::move(event)](jsi::Runtime &runtime) {
                auto payload = jsi::Object(runtime);
                {
                    auto insets = jsi::Object(runtime);
                    insets.setProperty(runtime, "top", event.top);
                    insets.setProperty(runtime, "right", event.right);
                    insets.setProperty(runtime, "bottom", event.bottom);
                    insets.setProperty(runtime, "left", event.left);
                    payload.setProperty(runtime, "safeAreaViewInsetsValue", insets);
                }
                return payload;
            });
        }

    } // namespace react
} // namespace facebook
