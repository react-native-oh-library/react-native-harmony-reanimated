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

#pragma once
#include <glog/logging.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNCSafeAreaProviderEventEmitters.h"

using namespace facebook;
namespace rnoh {

    enum SafeAreaEventType { SAFE_AREA_INSETS_CHANGE = 0 };

    SafeAreaEventType getSafeAreaEventType(ArkJS &arkJs, napi_value eventObject)
    {
        auto eventType = arkJs.getString(arkJs.getObjectProperty(eventObject, "type"));
        if (eventType == "onInsetsChange") {
            return SafeAreaEventType::SAFE_AREA_INSETS_CHANGE;
        } else {
            throw std::runtime_error("Unknown Page event type");
        }
    }

    class SafeAreaEventEmitRequestHandler : public EventEmitRequestHandler {
    public:
        void handleEvent(EventEmitRequestHandler::Context const &ctx) override
        {
            ArkJS arkJs(ctx.env);
            auto eventEmitter =
                ctx.shadowViewRegistry->getEventEmitter<react::RNCSafeAreaProviderEventEmitter>(ctx.tag);
            if (eventEmitter == nullptr) {
                return;
            }
            switch (getSafeAreaEventType(arkJs, ctx.payload)) {
                case SafeAreaEventType::SAFE_AREA_INSETS_CHANGE: {
                    RNOHNapiObject onInsetsChangeInsets =
                    arkJs.getObject(arkJs.getObjectProperty(ctx.payload, "insets"));
                    double top = arkJs.getDouble(onInsetsChangeInsets.getProperty("top"));
                    double right = arkJs.getDouble(onInsetsChangeInsets.getProperty("right"));
                    double bottom = arkJs.getDouble(onInsetsChangeInsets.getProperty("bottom"));
                    double left = arkJs.getDouble(onInsetsChangeInsets.getProperty("left"));
                    RNOHNapiObject onInsetsChangeFrame = arkJs.getObject(arkJs.getObjectProperty(ctx.payload, "frame"));
                    double x = arkJs.getDouble(onInsetsChangeFrame.getProperty("x"));
                    double y = arkJs.getDouble(onInsetsChangeFrame.getProperty("y"));
                    double width = arkJs.getDouble(onInsetsChangeFrame.getProperty("width"));
                    double height = arkJs.getDouble(onInsetsChangeFrame.getProperty("height"));
                    react::RNCSafeAreaProviderEventEmitter::OnInsetsChange event = {{top, right, bottom, left},
                                                                                    {x, y, width, height}};
                    eventEmitter->onInsetsChange(event);
                    break;
            }
                default:
                    break;
            }
        }
    };
} // namespace rnoh
