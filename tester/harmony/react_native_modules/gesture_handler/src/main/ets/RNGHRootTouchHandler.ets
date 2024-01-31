import { Tag } from "rnoh"
import { GestureHandlerRegistry } from "./GestureHandlerRegistry"
import { GestureHandlerArkUIAdapter } from "./GestureHandlerArkUIAdapter"
import { ViewRegistry } from "./ViewRegistry"
import { RNGHLogger } from './RNGHLogger'


export class RNGHRootTouchHandler {
  private adapterByViewTag: Map<number, GestureHandlerArkUIAdapter> = new Map() // TODO: remove an adapter when a view or gesture handler is removed
  private activeViewTags: number[] = []
  private viewRegistry: ViewRegistry
  private gestureHandlerRegistry: GestureHandlerRegistry
  private logger: RNGHLogger
  private rootTag: Tag

  constructor(rootTag: Tag, viewRegistry: ViewRegistry, gestureHandlerRegistry: GestureHandlerRegistry, logger: RNGHLogger) {
    this.rootTag = rootTag
    this.viewRegistry = viewRegistry
    this.gestureHandlerRegistry = gestureHandlerRegistry
    this.logger = logger
  }

  public handleTouch(e: TouchEvent) {
    if (e.type === TouchType.Down) {
      this.activeViewTags = []
    }
    for (const changedTouch of e.changedTouches) {
      const views = this.viewRegistry.getTouchableViewsAt({
        x: changedTouch.windowX,
        y: changedTouch.windowY
      }, this.rootTag)
      for (const view of views) {
        for (const handler of this.gestureHandlerRegistry.getGestureHandlersByViewTag(view.getTag())) {
          this.logger.info(`Found GestureHandler ${handler.getTag()} for view ${view.getTag()}`)
          if (!this.adapterByViewTag.has(view.getTag()))
            this.adapterByViewTag.set(view.getTag(), new GestureHandlerArkUIAdapter(handler, view, this.logger.cloneWithPrefix("ArkUIAdapter")))
          if (!this.activeViewTags.includes(view.getTag())) {
            const adapter = this.adapterByViewTag.get(view.getTag())
            if (adapter) {
              adapter.handleTouch(e)
            } else {
              console.warn("RNGH: Couldn't find adapter")
            }
          }
          if (e.type === TouchType.Down) {
            this.activeViewTags.push(view.getTag())
          }
        }
      }
      for (const viewTag of this.activeViewTags) {
        const adapter = this.adapterByViewTag.get(viewTag)
        if (adapter) {
          adapter.handleTouch(e)
        }
      }
    }
  }
}