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

#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include "Props.h"

namespace facebook {
    namespace react {

        RNCSafeAreaProviderProps::RNCSafeAreaProviderProps(const PropsParserContext &context,
                                                           const RNCSafeAreaProviderProps &sourceProps,
                                                           const RawProps &rawProps)
            : ViewProps(context, sourceProps, rawProps) {}

        RNCSafeAreaViewProps::RNCSafeAreaViewProps(const PropsParserContext &context,
                                                   const RNCSafeAreaViewProps &sourceProps, const RawProps &rawProps)
            : ViewProps(context, sourceProps, rawProps),

              mode(convertRawProp(context, rawProps, "mode", sourceProps.mode, {RNCSafeAreaViewMode::PADDING})),
              edges(convertRawProp(context, rawProps, "edges", sourceProps.edges, {})),
              paddingTop(convertRawProp(context, rawProps, "paddingTop", sourceProps.paddingTop, {0.0}))

        {}

        folly::dynamic toObject(const RNCSafeAreaViewEdgesStruct &value)
        {
            return folly::dynamic::object("top", value.top)("right", value.right)("bottom", value.bottom)("left", value.left);
        }

        std::string toString(const RNCSafeAreaViewMode &value)
        {
            switch (value) {
                case RNCSafeAreaViewMode::PADDING:
                    return "padding";
                case RNCSafeAreaViewMode::MARGIN:
                    return "margin";
            }
        }

        void fromRawValue(const PropsParserContext &context, const RawValue &value, RNCSafeAreaViewMode &result)
        {
            auto string = (std::string)value;
            if (string == "padding") {
                result = RNCSafeAreaViewMode::PADDING;
                return;
            }
            if (string == "margin") {
                result = RNCSafeAreaViewMode::MARGIN;
                return;
            }
        }

        void fromRawValue(const PropsParserContext &context, const RawValue &value,
                          RNCSafeAreaViewEdgesStruct &result)
        {
            auto map = (butter::map<std::string, RawValue>)value;
            auto tmp_top = map.find("top");
            if (tmp_top != map.end()) {
                fromRawValue(context, tmp_top->second, result.top);
            }
            auto tmp_right = map.find("right");
            if (tmp_right != map.end()) {
                fromRawValue(context, tmp_right->second, result.right);
            }
            auto tmp_bottom = map.find("bottom");
            if (tmp_bottom != map.end()) {
                fromRawValue(context, tmp_bottom->second, result.bottom);
            }
            auto tmp_left = map.find("left");
            if (tmp_left != map.end()) {
                fromRawValue(context, tmp_left->second, result.left);
            }
        }
    } // namespace react
} // namespace facebook
