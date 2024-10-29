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


import window from '@ohos.window';
import type { TurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { TurboModule } from '@rnoh/react-native-openharmony/ts';
import type { EdgeInsets, Event, Frame } from './common/SafeAreaType';
import Logger from './Logger';

const TAG: string = '[RNOH]SafeAreaViewTurboModule'


export class SafeAreaViewTurboModule extends TurboModule {
  private windowInstance: window.Window = null;

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    if (!this.windowInstance) {
      window.getLastWindow(this.ctx.uiAbilityContext, (err, data) => {
        if (err.code) {
          Logger.debug(TAG, 'Failed to obtain the top window. Cause: ' + JSON.stringify(err));
          return;
        }
        this.windowInstance = data
      })
      this.sleep(10);
    }
    Logger.debug(TAG, '[RNOH]:SafeAreaViewTurboModule constructor');
  }

  sleep(delay: number) {
    var start = new Date().getTime();
    while ((new Date().getTime() - start) < delay) {
      continue;
    }
  }

  getConstants(): {} {
    Logger.debug(TAG, 'RNCSafeAreaContext getConstants:' + this.windowInstance);
    let frame: Frame = {
      x: 0,
      y: 0,
      width: 0,
      height: 0
    }
    let insets: EdgeInsets = {
      top: 0,
      right: 0,
      bottom: 0,
      left: 0
    }
    if (this.windowInstance != null) {
      const windowInfo = this.windowInstance.getWindowProperties()
      const avoidArea = this.windowInstance.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
      const avoidAreaBottom = this.windowInstance.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR);
      frame = {
        x: px2vp(windowInfo.windowRect.left),
        y: px2vp(windowInfo.windowRect.top),
        width: px2vp(windowInfo.windowRect.width),
        height: px2vp(windowInfo.windowRect.height)
      }
      insets = {
        top: px2vp(avoidArea.topRect.height),
        right: px2vp(avoidArea.rightRect.height),
        bottom: px2vp(avoidAreaBottom.bottomRect.height),
        left: px2vp(avoidArea.leftRect.height)
      }
    }
    return { "initialWindowMetrics": { frame, insets } }
  }

  getSafeAreaInsets(): Promise<Event> {
    return new Promise(async (resolve, reject) => {
      try {
        const windowInstance = await window.getLastWindow(this.ctx.uiAbilityContext);
        const windowInfo = windowInstance.getWindowProperties()
        Logger.debug(TAG, JSON.stringify(windowInfo));
        const frame: Frame = {
          x: px2vp(windowInfo.windowRect.left),
          y: px2vp(windowInfo.windowRect.top),
          width: px2vp(windowInfo.windowRect.width),
          height: px2vp(windowInfo.windowRect.height)
        }
        if (!windowInfo.isLayoutFullScreen) {
          const insets: EdgeInsets = {
            top: 0,
            right: 0,
            bottom: 0,
            left: 0
          }
          resolve({ insets, frame })
        } else {
          let type = window.AvoidAreaType.TYPE_SYSTEM;
          let avoidArea = windowInstance.getWindowAvoidArea(type);
          const avoidAreaBottom = this.windowInstance.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR);
          const insets: EdgeInsets = {
            top: px2vp(avoidArea.topRect.height),
            right: px2vp(avoidArea.rightRect.height),
            bottom: px2vp(avoidAreaBottom.bottomRect.height),
            left: px2vp(avoidArea.leftRect.height)
          }
          resolve({ insets, frame })
        }
      } catch (exception) {
        reject(exception)
      }
    })
  }
}
