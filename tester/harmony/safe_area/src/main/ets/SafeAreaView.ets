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

import matrix4 from '@ohos.matrix4'

import {
  Descriptor,
  ComponentBuilderContext,
  RNOHContext,
  ViewBaseProps,
  Tag,
  RNComponentFactory,
  ViewRawProps,
  RNViewManager,
  ViewDescriptorWrapperBase,
  TransformMatrix,
  CompactValue,
  Edges,
  Point
} from '@rnoh/react-native-openharmony'

import { SafeAreaViewModifier } from './SafeAreaViewModifier'
import { SafeAreaViewTurboModule } from "./SafeViewTurboModule"
import { EdgeInsets, AvoidArea, Event, EdgesMode } from './common/SafeAreaType';
import Logger from './Logger'

export const SAFE_AREA_VIEW_TYPE: string = "RNCSafeAreaView"

export type DimensionValue = number | 'auto' | `${number}%` | null;

export interface SafeAreaViewProps extends ViewBaseProps {
  edges: EdgesMode,
  mode: string,
  margin: EdgeInsets,
  padding: EdgeInsets,
}

export interface SafeAreaViewState {}

export interface SafeAreaViewRawProps extends ViewRawProps {
  mode: string
}

export type SafeAreaViewDescriptor = Descriptor<"RNCSafeAreaView", SafeAreaViewProps, SafeAreaViewState, SafeAreaViewRawProps>;

export class SafeAreaViewManager extends RNViewManager {
  protected offsetX: number = 0;
  protected offsetY: number = 0;

  constructor(
    tag: Tag,
    ctx: RNOHContext,
    offsetX: number,
    offsetY: number
  ) {
    super(tag, ctx);
    this.offsetX = offsetX
    this.offsetY = offsetY
  }

  setOffsetX(offsetX: number) {
    this.offsetX = offsetX
  }

  setOffsetY(offsetY: number) {
    this.offsetY = offsetY
  }

  public computeChildPoint(point: Point, childTag: Tag): Point {
    return super.computeChildPoint({ x: point.x + this.offsetX, y: point.y + this.offsetY }, childTag);
  }
}

export class SafeAreaViewDescriptorWrapper extends ViewDescriptorWrapperBase<"RNCSafeAreaView", SafeAreaViewProps, SafeAreaViewState, SafeAreaViewRawProps> {
  public edgeInsets: EdgeInsets = { top: 0,
    right: 0,
    bottom: 0,
    left: 0 }

  public get mode(): string | undefined {
    return this.rawProps.mode
  }

  public SetEdgeInsets(edges: EdgeInsets) {
    this.edgeInsets = edges
  }

  public getEdgeInsets() {
    return this.edgeInsets
  }

  public get margin(): Edges<CompactValue> {
    const resolvedEdges = this.resolveEdges({
      all: this.rawProps.margin,
      top: this.rawProps.marginTop,
      left: this.rawProps.marginLeft,
      right: this.rawProps.marginRight,
      bottom: this.rawProps.marginBottom,
      start: this.rawProps.marginStart,
      end: this.rawProps.marginEnd,
    })

    return {
      top: new CompactValue(resolvedEdges.top as DimensionValue, this.height),
      left: new CompactValue(resolvedEdges.left as DimensionValue, this.width),
      right: new CompactValue(resolvedEdges.right as DimensionValue, this.width),
      bottom: new CompactValue(resolvedEdges.bottom as DimensionValue, this.height),
    }
  }
}

@Component
export struct SafeAreaView {
  ctx!: RNOHContext
  tag: number = 0
  @BuilderParam public buildCustomComponent: (componentBuilderContext: ComponentBuilderContext) => void
  private componentManager!: SafeAreaViewManager
  @State descriptor: SafeAreaViewDescriptor = {} as SafeAreaViewDescriptor
  @State descriptorWrapper: SafeAreaViewDescriptorWrapper | undefined = undefined
  private unregisterDescriptorChangesListener?: () => void = undefined
  private unregisterComponentManager?: () => void = undefined
  @State avoidArea: AvoidArea = {} as AvoidArea
  @State edgeInsets: EdgeInsets = { top: 0, right: 0, bottom: 0, left: 0 }

  getEdgeValue(edgeMode: string | undefined, insetValue: number, edgeValue: number): number {
    if (edgeMode == 'off') {
      return 0;
    } else if (edgeMode == 'maximum') {
      //insetValue 安全区域    edgeValue原区域
      return edgeValue > insetValue ? edgeValue - insetValue : insetValue - edgeValue
    } else {
      return insetValue;
    }
  }

  updateInsets() {
    const safeAreaViewTurboModule = this.ctx.rnInstance.getTurboModule<SafeAreaViewTurboModule>("RNCSafeAreaContext")
    const safeAreaInsets = safeAreaViewTurboModule.getSafeAreaInsets();
    let edges: EdgesMode = { top: 'additive', right: 'additive', bottom: 'additive', left: 'additive' };
    edges = this.descriptor.props.edges
    safeAreaInsets.then((data: Event) => {
      if (data.insets) {
        let edgesData = this.descriptorWrapper?.mode && this.descriptorWrapper?.mode === 'margin' ? this.descriptor.props.margin : this.descriptor.props.padding
        this.edgeInsets = {
          top: this.getEdgeValue(edges.top, data.insets.top, edgesData?.top),
          right: this.getEdgeValue(edges.right, data.insets.right, edgesData?.right),
          bottom: this.getEdgeValue(edges.bottom, data.insets.bottom, edgesData?.bottom),
          left: this.getEdgeValue(edges.left, data.insets.left, edgesData?.left)
        }
      }
      Logger.info("edgeInsets " + JSON.stringify(this.edgeInsets))
    })
      .catch((e: string | Error) => {
        Logger.error('updateInsets error', JSON.stringify(e));
        this.edgeInsets = {
          top: 0,
          right: 0,
          bottom: 0,
          left: 0
        }
      })
      .finally(() => {
        if (this.descriptorWrapper) {
          this.descriptorWrapper.SetEdgeInsets(this.edgeInsets)
        }
        let offsetY = this.edgeInsets.bottom - this.edgeInsets.top
        let offsetX = this.edgeInsets.right - this.edgeInsets.left
        this.componentManager.setOffsetY(offsetY)
        this.componentManager.setOffsetX(offsetX)
        this.componentManager.markBoundingBoxAsDirty()
      })
  }

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<SafeAreaViewDescriptor>(this.tag)
    if (this.descriptor) {
      this.setDescriptor(this.descriptor)
    }
    this.updateInsets()
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.setDescriptor(newDescriptor as SafeAreaViewDescriptor)
        this.updateInsets()
      }
    )
    this.initComponent()
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
    this.unregisterComponentManager?.()
  }

  initComponent() {
    if (!this.componentManager) {
      this.componentManager = new SafeAreaViewManager(this.tag, this.ctx, 0, 0)
    }
    this.unregisterComponentManager = this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager)
    const parentTag = this.descriptor.parentTag;
    this.componentManager.setParentTag(parentTag);
  }

  private setDescriptor(descriptor: SafeAreaViewDescriptor) {
    this.descriptorWrapper = new SafeAreaViewDescriptorWrapper(descriptor)
    this.descriptor = descriptor
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
    .attributeModifier(SafeAreaViewModifier.getInstance().setDescriptor(this.descriptorWrapper))
  }
}
