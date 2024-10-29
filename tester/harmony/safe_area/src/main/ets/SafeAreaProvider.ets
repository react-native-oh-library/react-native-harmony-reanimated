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

import {
  Descriptor,
  ComponentBuilderContext,
  RNComponentFactory,
  RNOHContext,
  ViewRawProps,
  ViewBaseProps,
  ViewDescriptorWrapperBase,
  RNViewManager,
  ViewBaseDescriptor
} from '@rnoh/react-native-openharmony'

import { SafeAreaViewTurboModule } from "./SafeViewTurboModule"
import { Event, Tag } from './common/SafeAreaType';
import Logger from './Logger'
import { SafeAreaProviderModifier } from './SafeAreaProviderModifier'

export const SAFE_AREA_PROVIDER_TYPE: string = "RNCSafeAreaProvider"

export interface SafeAreaProviderProps extends ViewBaseProps {}

export interface SafeAreaProviderState {}

export interface SafeAreaProviderRawProps extends ViewRawProps {}

export type SafeAreaProviderDescriptor = Descriptor<"RNCSafeAreaProvider", SafeAreaProviderProps, SafeAreaProviderState, SafeAreaProviderRawProps>

export class SafeAreaProviderDescriptorWrapper extends ViewDescriptorWrapperBase<"RNCSafeAreaProvider", SafeAreaProviderProps, SafeAreaProviderState, SafeAreaProviderRawProps> {
}

@Component
export struct SafeAreaProvider {
  ctx!: RNOHContext
  tag: number = 0
  @BuilderParam buildCustomComponent: (componentBuilderContext: ComponentBuilderContext) => void
  private componentManager!: RNViewManager
  @State descriptor: SafeAreaProviderDescriptor = {} as SafeAreaProviderDescriptor
  @State descriptorWrapper: SafeAreaProviderDescriptorWrapper | undefined = undefined
  private unregisterDescriptorChangesListener?: () => void = undefined
  private unregisterComponentManager?: () => void = undefined

  onInsetsChange() {
    const safeAreaViewTurboModule = this.ctx.rnInstance.getTurboModule<SafeAreaViewTurboModule>("RNCSafeAreaContext")
    const safeAreaInsets = safeAreaViewTurboModule.getSafeAreaInsets();
    safeAreaInsets.then((data: Event) => {
      this.ctx.rnInstance.emitComponentEvent(this.descriptor.tag, SAFE_AREA_PROVIDER_TYPE, {
        type: "onInsetsChange",
        insets: data.insets,
        frame: {
          x: this.descriptor.layoutMetrics.frame.origin.x,
          y: this.descriptor.layoutMetrics.frame.origin.y,
          width: this.descriptor.layoutMetrics.frame.size.width,
          height: this.descriptor.layoutMetrics.frame.size.height
        },
      })
    })
  }

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<SafeAreaProviderDescriptor>(this.tag)
    if (this.descriptor) {
      this.setDescriptor(this.descriptor)
    }
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.setDescriptor(newDescriptor as SafeAreaProviderDescriptor)
      }
    )
    this.initComponent();
    this.onInsetsChange()
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
    this.unregisterComponentManager?.()
  }

  initComponent() {
    if (!this.componentManager) {
      this.componentManager = new RNViewManager(this.tag, this.ctx)
    }
    this.unregisterComponentManager = this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager)
    const parentTag = this.descriptor.parentTag;
    this.componentManager.setParentTag(parentTag);
  }

  private setDescriptor(descriptor: SafeAreaProviderDescriptor) {
    this.descriptorWrapper = new SafeAreaProviderDescriptorWrapper(descriptor)
    Logger.info('descriptorWrapper' + JSON.stringify(this.descriptorWrapper))
  }

  build() {
    Stack() {
      ForEach(this.descriptorWrapper?.childrenTags, (childrenTag: Tag) => {
        RNComponentFactory({
          ctx: this.ctx,
          tag: childrenTag,
          buildCustomComponent: this.buildCustomComponent
        })
      }, (tag: Tag) => tag.toString())
    }
    .attributeModifier(SafeAreaProviderModifier.getInstance().setDescriptor(this.descriptorWrapper))
  }
}
