import { RNInstance } from "rnoh/ts"
import { GestureStateChangeEvent, GestureUpdateEvent, GestureTouchEvent } from "./OutgoingEvent"
import { RNGHLogger } from './RNGHLogger'

export interface EventDispatcher {
  onGestureHandlerStateChange(event: GestureStateChangeEvent): void
  onGestureHandlerEvent(event: GestureStateChangeEvent | GestureUpdateEvent | GestureTouchEvent): void
}

export class JSEventDispatcher implements EventDispatcher {
  constructor(private rnInstance: RNInstance, private logger: RNGHLogger) {
  }

  public onGestureHandlerStateChange(event: GestureStateChangeEvent) {
    this.logger.info(`onGestureHandlerStateChange`)
    this.rnInstance.emitDeviceEvent("onGestureHandlerStateChange", event)
  }

  public onGestureHandlerEvent(event: GestureStateChangeEvent | GestureUpdateEvent | GestureTouchEvent) {
    this.logger.info(`onGestureHandlerEvent`)
    this.rnInstance.emitDeviceEvent("onGestureHandlerEvent", event)
  }
}

export class AnimatedEventDispatcher implements EventDispatcher {
  constructor(private rnInstance: RNInstance, private logger: RNGHLogger, private viewTag: number) {
  }

  public onGestureHandlerStateChange(event: GestureStateChangeEvent) {
    this.logger.info(`onGestureHandlerStateChange`)
    this.rnInstance.emitDeviceEvent("onGestureHandlerStateChange", event)
  }

  public onGestureHandlerEvent(event: GestureStateChangeEvent | GestureUpdateEvent | GestureTouchEvent) {
    this.logger.info(`onGestureHandlerEvent`)
    this.rnInstance.emitComponentEvent(this.viewTag, "onGestureHandlerEvent", event)
  }
}