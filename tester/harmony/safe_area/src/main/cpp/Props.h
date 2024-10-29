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
#include <jsi/jsi.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>

namespace facebook {
    namespace react {

        class JSI_EXPORT RNCSafeAreaProviderProps final : public ViewProps {
        public:
            RNCSafeAreaProviderProps() = default;
            RNCSafeAreaProviderProps(const PropsParserContext &context, const RNCSafeAreaProviderProps &sourceProps,
                                     const RawProps &rawProps);

#pragma mark - Props
        };

        enum class RNCSafeAreaViewMode { PADDING, MARGIN };

        struct RNCSafeAreaViewEdgesStruct {
            std::string top;
            std::string right;
            std::string bottom;
            std::string left;
        };

        std::string toString(const RNCSafeAreaViewMode &value);
        folly::dynamic toObject(const RNCSafeAreaViewEdgesStruct &value);
        void fromRawValue(const PropsParserContext &context, const RawValue &value, RNCSafeAreaViewMode &result);
        void fromRawValue(const PropsParserContext &context, const RawValue &value, RNCSafeAreaViewEdgesStruct &result);


        class JSI_EXPORT RNCSafeAreaViewProps final : public ViewProps {
        public:
            RNCSafeAreaViewProps() = default;
            RNCSafeAreaViewProps(const PropsParserContext &context, const RNCSafeAreaViewProps &sourceProps,
                                 const RawProps &rawProps);

#pragma mark - Props

            RNCSafeAreaViewMode mode{RNCSafeAreaViewMode::PADDING};
            RNCSafeAreaViewEdgesStruct edges{};
            double paddingTop{0.0};
        };

    } // namespace react
} // namespace facebook
