import { Point } from "rnoh"
import { GestureHandler } from "./GestureHandler"
import { AdaptedEvent, EventType, PointerType, TouchEventType, Touch } from "./Event"
import { View } from "./View"
import { RNGHLogger } from "./RNGHLogger"

export class GestureHandlerArkUIAdapter {
  private activePointerIds = new Set<number>()
  private pointersIdInBounds = new Set<number>()
  private gestureHandler: GestureHandler
  private view: View
  private logger: RNGHLogger

  constructor(gestureHandler: GestureHandler, view: View, logger: RNGHLogger) {
    logger.info("constructor")
    this.gestureHandler = gestureHandler
    this.view = view
    this.logger = logger
  }

  handleTouch(e: TouchEvent) {
    for (const changedTouch of e.changedTouches) {
      if (this.shouldSkipTouch(changedTouch)) continue;
      const wasInBounds = this.pointersIdInBounds.has(changedTouch.id)
      const isInBounds = this.isInBounds({ x: changedTouch.windowX, y: changedTouch.windowY })
      this.logger.info(`handleTouch: ${JSON.stringify({ type: changedTouch.type, wasInBounds, isInBounds })}`)
      const adaptedEvent = this.adaptTouchEvent(e, changedTouch)
      switch (adaptedEvent.eventType) {
        case EventType.DOWN:
          this.gestureHandler.onPointerDown(adaptedEvent)
          break;
        case EventType.ADDITIONAL_POINTER_DOWN:
          this.gestureHandler.onAdditionalPointerAdd(adaptedEvent)
          break;
        case EventType.UP:
          this.gestureHandler.onPointerUp(adaptedEvent)
          break;
        case EventType.ADDITIONAL_POINTER_UP:
          this.gestureHandler.onAdditionalPointerRemove(adaptedEvent)
          break;
        case EventType.MOVE:
          if (!wasInBounds && !isInBounds)
            this.gestureHandler.onPointerOutOfBounds(adaptedEvent)
          else
            this.gestureHandler.onPointerMove(adaptedEvent)
          break;
        case EventType.ENTER:
          this.gestureHandler.onPointerEnter(adaptedEvent)
          break;
        case EventType.OUT:
          this.gestureHandler.onPointerOut(adaptedEvent)
          break;
        case EventType.CANCEL:
          this.gestureHandler.onPointerCancel(adaptedEvent)
          break;
      }
    }
  }

  private shouldSkipTouch(changedTouch: TouchObject): boolean {
    return changedTouch.type === TouchType.Down && !this.isInBounds({
      x: changedTouch.windowX,
      y: changedTouch.windowY
    })
  }

  private adaptTouchEvent(e: TouchEvent, changedTouch: TouchObject): AdaptedEvent {
    const xAbsolute = changedTouch.windowX
    const yAbsolute = changedTouch.windowY

    const eventType = this.mapTouchTypeToEventType(
      changedTouch.type,
      this.activePointerIds.size,
      this.isInBounds({ x: xAbsolute, y: yAbsolute }),
      this.pointersIdInBounds.has(changedTouch.id)
    )
    this.updateIsInBoundsByPointerId(
      changedTouch.type,
      changedTouch.id,
      xAbsolute,
      yAbsolute
    )
    this.updateActivePointers(changedTouch.type, changedTouch.id)
    return {
      x: xAbsolute,
      y: yAbsolute,
      offsetX: xAbsolute - this.view.getBoundingRect().x,
      offsetY: yAbsolute - this.view.getBoundingRect().y,
      pointerId: changedTouch.id,
      eventType: eventType,
      pointerType: PointerType.TOUCH,
      buttons: 0,
      time: e.timestamp,
      allTouches: e.touches.map((touch) => this.mapTouchObjectToTouch(touch)),
      changedTouches: e.changedTouches.map((touch) => this.mapTouchObjectToTouch(touch)),
      touchEventType: this.mapTouchTypeToTouchEventType(changedTouch.type)
    }
  }

  private updateIsInBoundsByPointerId(
    touchType: TouchType,
    pointerId: number,
    x: number,
    y: number
  ) {
    switch (touchType) {
      case TouchType.Down:
        if (this.isInBounds({ x, y }))
          this.pointersIdInBounds.add(pointerId)
        break;
      case TouchType.Move:
        if (this.isInBounds({ x, y }))
          this.pointersIdInBounds.add(pointerId)
        else
          this.pointersIdInBounds.delete(pointerId)
        break;
      case TouchType.Up:
        this.pointersIdInBounds.delete(pointerId)
        break;
      case TouchType.Cancel:
        this.pointersIdInBounds.delete(pointerId)
        break;
    }
  }

  private isInBounds(point: Point): boolean {
    const x = point.x
    const y = point.y
    const rect = this.view.getBoundingRect()
    const result = x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height)
    return result;
  }

  private updateActivePointers(touchType: TouchType, pointerId: number): void {
    switch (touchType) {
      case TouchType.Down:
        this.activePointerIds.add(pointerId)
      case TouchType.Up:
        this.activePointerIds.delete(pointerId)
      case TouchType.Cancel:
        this.activePointerIds.clear()
      default:
        return
    }
  }

  private mapTouchObjectToTouch(touchObject: TouchObject): Touch {
    return {
      id: touchObject.id,
      x: touchObject.x,
      y: touchObject.y,
      absoluteX: touchObject.windowX,
      absoluteY: touchObject.windowY
    }
  }

  private mapTouchTypeToEventType(
    touchType: TouchType,
    activePointersCounter: number,
    isCurrentlyInBounds: boolean,
    wasInBounds: boolean
  ): EventType {
    switch (touchType) {
      case TouchType.Down:
        if (activePointersCounter > 0)
          return EventType.ADDITIONAL_POINTER_DOWN
        else
          return EventType.DOWN
      case TouchType.Up:
        if (activePointersCounter > 1)
          return EventType.ADDITIONAL_POINTER_UP
        else
          return EventType.UP
      case TouchType.Move:
        if (isCurrentlyInBounds) {
          return wasInBounds ? EventType.MOVE : EventType.ENTER
        } else {
          return wasInBounds ? EventType.OUT : EventType.MOVE
        }
      case TouchType.Cancel:
        return EventType.CANCEL
      default:
        console.error("RNGH", "Unknown touchType:", touchType)
        throw new Error("Unknown touchType")
    }
  }

  private mapTouchTypeToTouchEventType(touchType: TouchType): TouchEventType {
    switch (touchType) {
      case TouchType.Down:
        return TouchEventType.DOWN
      case TouchType.Up:
        return TouchEventType.UP
      case TouchType.Move:
        return TouchEventType.MOVE
      case TouchType.Cancel:
        return TouchEventType.CANCELLED
      default:
        return TouchEventType.UNDETERMINED
    }
  }
}