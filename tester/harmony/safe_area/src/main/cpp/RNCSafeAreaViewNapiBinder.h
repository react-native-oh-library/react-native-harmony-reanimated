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
#include <react/renderer/components/view/conversions.h>
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"
#include "Props.h"


namespace rnoh {
    class RNCSafeAreaViewNapiBinder : public ViewComponentNapiBinder {
    public:
        napi_value createProps(napi_env env, facebook::react::ShadowView const shadowView) override
        {
            napi_value napiViewProps = ViewComponentNapiBinder::createProps(env, shadowView);
            if (auto props = std::dynamic_pointer_cast<const facebook::react::RNCSafeAreaViewProps>(shadowView.props)) {
                return ArkJS(env)
                    .getObjectBuilder(napiViewProps)
                    .addProperty("mode", facebook::react::toString(props->mode))
                    .addProperty("edges", facebook::react::toObject(props->edges))
                    .addProperty("padding", getParagraphPaddingProps(shadowView))
                    .addProperty("margin", getParagraphMarginProps(shadowView))
                    .build();
            }
            return napiViewProps;
        };

        folly::dynamic getParagraphMarginProps(facebook::react::ShadowView const shadowView)
        {
            float top = 0;
            float right = 0;
            float bottom = 0;
            float left = 0;
            auto isRTL =
                bool{shadowView.layoutMetrics.layoutDirection == facebook::react::LayoutDirection::RightToLeft};
            if (auto props = std::dynamic_pointer_cast<const facebook::react::RNCSafeAreaViewProps>(shadowView.props)) {
                auto yogaMargin = props->yogaStyle.margin();
                if (facebook::react::optionalFloatFromYogaValue(yogaMargin[YGEdgeAll]).has_value()) {
                    float padding = facebook::react::optionalFloatFromYogaValue(yogaMargin[YGEdgeAll]).value();
                    top = right = bottom = left = padding;
                }
                setHorizontalPadding(yogaMargin, isRTL, left, right);
                setVerticalPadding(yogaMargin, top, bottom);
            }
            return folly::dynamic::object("top", top)("right", right)("bottom", bottom)("left", left);
        };

        folly::dynamic getParagraphPaddingProps(facebook::react::ShadowView const shadowView)
        {
            float top = 0;
            float right = 0;
            float bottom = 0;
            float left = 0;
            auto isRTL =
                bool{shadowView.layoutMetrics.layoutDirection == facebook::react::LayoutDirection::RightToLeft};
            if (auto props = std::dynamic_pointer_cast<const facebook::react::RNCSafeAreaViewProps>(shadowView.props)) {
                auto yogaPadding = props->yogaStyle.padding();
                if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeAll]).has_value()) {
                    float padding = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeAll]).value();
                    top = right = bottom = left = padding;
                }
                setHorizontalPadding(yogaPadding, isRTL, left, right);
                setVerticalPadding(yogaPadding, top, bottom);
            }
            return folly::dynamic::object("top", top)("right", right)("bottom", bottom)("left", left);
        };

        void setHorizontalPadding(YGStyle::Edges const &yogaPadding, bool isRTL, float &left, float &right)
        {
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeLeft]).has_value()) {
                left = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeLeft]).value();
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeRight]).has_value()) {
                right = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeRight]).value();
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeStart]).has_value()) {
                float paddingStart = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeStart]).value();
                isRTL ? (right = paddingStart) : (left = paddingStart);
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeEnd]).has_value()) {
                float paddingEnd = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeEnd]).value();
                isRTL ? (left = paddingEnd) : (right = paddingEnd);
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeHorizontal]).has_value()) {
                float paddingHorizontal =
                    facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeHorizontal]).value();
                left = right = paddingHorizontal;
            }
        }

        void setVerticalPadding(YGStyle::Edges const &yogaPadding, float &top, float &bottom)
        {
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeTop]).has_value()) {
                top = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeTop]).value();
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeBottom]).has_value()) {
                bottom = facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeBottom]).value();
            }
            if (facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeVertical]).has_value()) {
                float paddingVertical =
                    facebook::react::optionalFloatFromYogaValue(yogaPadding[YGEdgeVertical]).value();
                top = bottom = paddingVertical;
            }
        }
    };
} // namespace rnoh
