import { TurboModule, TurboModuleContext } from 'rnoh/ts';
import { GestureHandlerRegistry } from './GestureHandlerRegistry';
import { GestureHandlerFactory } from "./GestureHandlerFactory"
import { ViewRegistry } from './ViewRegistry';
import { RNGHLogger, StandardRNGHLogger, FakeRNGHLogger } from './RNGHLogger';
import { EventDispatcher, JSEventDispatcher, AnimatedEventDispatcher } from './EventDispatcher'
import { RNOHScrollLocker } from "./RNOHScrollLocker"

export enum ActionType {
  REANIMATED_WORKLET = 1,
  NATIVE_ANIMATED_EVENT = 2,
  JS_FUNCTION_OLD_API = 3,
  JS_FUNCTION_NEW_API = 4,
}


export class RNGestureHandlerModule extends TurboModule {
  static NAME = "RNGestureHandlerModule"

  private gestureHandlerRegistry = new GestureHandlerRegistry()
  private gestureHandlerFactory: GestureHandlerFactory | undefined = undefined
  private viewRegistry: ViewRegistry | undefined = undefined
  private logger: RNGHLogger

  constructor(ctx: TurboModuleContext) {
    super(ctx)
    const debug = false
    this.logger = debug ? new StandardRNGHLogger(ctx.logger, "RNGH") : new FakeRNGHLogger()
  }

  public install() {
    this.viewRegistry = new ViewRegistry(this.ctx.descriptorRegistry, this.ctx.componentManagerRegistry)
    this.gestureHandlerFactory = new GestureHandlerFactory(this.logger, new RNOHScrollLocker(this.ctx.rnInstance))
  }

  public createGestureHandler(
    handlerName: string,
    handlerTag: number,
    config: Readonly<Record<string, unknown>>
  ) {
    if (!this.gestureHandlerFactory) {
      this.ctx.logger.error("Trying to create a gesture handler before creating gesture handler factory")
      return
    }
    const gestureHandler = this.gestureHandlerFactory.create(handlerName, handlerTag)
    this.gestureHandlerRegistry.addGestureHandler(gestureHandler)
    gestureHandler.updateGestureConfig(config)
  }

  public attachGestureHandler(
    handlerTag: number,
    viewTag: number,
    actionType: ActionType
  ) {
    const eventDispatcher = this.createEventDispatcher(actionType, viewTag)
    if (!eventDispatcher) {
      this.ctx.logger.error("RNGH: Couldn't create EventDispatcher")
      return
    }
    const view = this.viewRegistry.getViewByTag(viewTag)
    if (!view) {
      this.ctx.logger.error(`RNGH: Couldn't attachGestureHandler to view ${viewTag}`)
      return;
    }
    this.gestureHandlerRegistry.bindGestureHandlerWithView(handlerTag, view)
    this.gestureHandlerRegistry
      .getGestureHandlerByHandlerTag(handlerTag)
      .setEventDispatcher(eventDispatcher)
  }

  private createEventDispatcher(actionType: ActionType, viewTag: number): EventDispatcher | null {
    switch (actionType) {
      case ActionType.REANIMATED_WORKLET:
        this.ctx.logger.error("RNGH: Reanimated Worklets are not supported")
        break;
      case ActionType.NATIVE_ANIMATED_EVENT:
        return new AnimatedEventDispatcher(this.ctx.rnInstance, this.logger.cloneWithPrefix('AnimatedEventDispatcher'), viewTag)
      case ActionType.JS_FUNCTION_OLD_API:
      case ActionType.JS_FUNCTION_NEW_API:
        return new JSEventDispatcher(this.ctx.rnInstance, this.logger.cloneWithPrefix('JSEventDispatcher'));
    }
    return null
  }

  public updateGestureHandler(
    handlerTag: number,
    newConfig: Readonly<Record<string, unknown>>
  ) {
    const gestureHandler = this.gestureHandlerRegistry.getGestureHandlerByHandlerTag(handlerTag)
    gestureHandler.updateGestureConfig(newConfig)
  }

  public dropGestureHandler(handlerTag: number) {
    this.warn("dropGestureHandler is not implemented")
  }

  public handleSetJSResponder(tag: number, blockNativeResponder: boolean) {
    this.warn("handleSetJSResponder is not implemented")
  }

  public handleClearJSResponder() {
    this.warn("handleClearJSResponder is not implemented")
  }

  public flushOperations() {
    this.warn("flushOperations is not implemented")
  }

  // -------------------------------------------------------------------------------------------------------------------
  protected warn(message: string) {
    this.ctx.logger.warn("RNGH: " + message)
  }

  public getGestureHandlerRegistry() {
    return this.gestureHandlerRegistry
  }

  public getLogger() {
    return this.logger
  }

  public getViewRegistry() {
    return this.viewRegistry
  }
}
