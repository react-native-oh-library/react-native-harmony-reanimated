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

#include "RNOH/Package.h"
#include "RNCSafeAreaContextTurbomodule.h"
#include "ComponentDescriptors.h"
#include "RNCSafeAreaViewJSIBinder.h"
#include "RNCSafeAreaProviderJSIBinder.h"
#include "RNCSafeAreaViewNapiBinder.h"
#include "RNCSafeAreaProviderNapiBinder.h"
#include "SafeAreaEventEmitRequestHandler.h"
#include "SafeAreaProviderComponentInstance.h"
#include "SafeAreaViewComponentInstance.h"

using namespace rnoh;
using namespace facebook;

class SafeAreaPackageComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
public:
    using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;

    ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
        if (ctx.componentName == "RNCSafeAreaProvider") {
            return std::make_shared<SafeAreaProviderComponentInstance>(std::move(ctx));
        } else if (ctx.componentName == "RNCSafeAreaView") {
            return std::make_shared<SafeAreaViewComponentInstance>(std::move(ctx));
        }
        return nullptr;
    }
};

class RNCSafeAreaContextFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override
    {
        if (name == "RNCSafeAreaContext") {
            return std::make_shared<RNCSafeAreaContextTurbomodule>(ctx, name);
        }
        return nullptr;
    };
};

namespace rnoh {

    class SafeAreaViewPackage : public Package {
    public:
        SafeAreaViewPackage(Package::Context ctx) : Package(ctx) {}

        ComponentInstanceFactoryDelegate::Shared createComponentInstanceFactoryDelegate() override {
            return std::make_shared<SafeAreaPackageComponentInstanceFactoryDelegate>();
        }

        std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override
        {
            return std::make_unique<RNCSafeAreaContextFactoryDelegate>();
        }
    
        std::vector<facebook::react::ComponentDescriptorProvider> createComponentDescriptorProviders() override
        {
            return {facebook::react::concreteComponentDescriptorProvider<
                        facebook::react::RNCSafeAreaProviderComponentDescriptor>(),
                    facebook::react::concreteComponentDescriptorProvider<
                        facebook::react::RNCSafeAreaViewComponentDescriptor>()};
        }

        ComponentJSIBinderByString createComponentJSIBinderByName() override
        {
            return {{"RNCSafeAreaProvider", std::make_shared<RNCSafeAreaProviderJSIBinder>()},
                    {"RNCSafeAreaView", std::make_shared<RNCSafeAreaViewJSIBinder>()}};
        };

        ComponentNapiBinderByString createComponentNapiBinderByName() override
        {
            return {{"RNCSafeAreaProvider", std::make_shared<RNCSafeAreaProviderNapiBinder>()},
                    {"RNCSafeAreaView", std::make_shared<RNCSafeAreaViewNapiBinder>()}};
        };

        EventEmitRequestHandlers createEventEmitRequestHandlers() override
        {
            return {std::make_shared<SafeAreaEventEmitRequestHandler>()};
        }
    };
} // namespace rnoh
