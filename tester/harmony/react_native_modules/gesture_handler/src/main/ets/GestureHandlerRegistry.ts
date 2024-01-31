import { GestureHandler } from "./GestureHandler"
import { View } from "./View"

export class GestureHandlerRegistry {
  private gestureHandlerByHandlerTag: Map<number, GestureHandler> = new Map()
  private gestureHandlersByViewTag: Map<number, Set<GestureHandler>> = new Map()

  public addGestureHandler(gestureHandler: GestureHandler) {
    this.gestureHandlerByHandlerTag.set(gestureHandler.getTag(), gestureHandler)
  }

  public bindGestureHandlerWithView(gestureHandlerTag: number, view: View) {
    const viewTag = view.getTag()
    if (!this.gestureHandlersByViewTag.has(viewTag))
      this.gestureHandlersByViewTag.set(viewTag, new Set())
    const gestureHandler = this.gestureHandlerByHandlerTag.get(gestureHandlerTag)
    this.gestureHandlersByViewTag.get(viewTag).add(gestureHandler)
    gestureHandler.onViewAttached(view)
  }

  public getGestureHandlersByViewTag(viewTag: number): GestureHandler[] {
    return Array.from(this.gestureHandlersByViewTag.get(viewTag) ?? [])
  }

  public getGestureHandlerByHandlerTag(handlerTag: number): GestureHandler {
    return this.gestureHandlerByHandlerTag.get(handlerTag)
  }
}