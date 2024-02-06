import { GestureHandlerOrchestrator } from './GestureHandlerOrchestrator';
import { GestureHandler, GestureHandlerDependencies, ScrollLocker } from "./GestureHandler"
import { PointerTracker } from './PointerTracker';
import { RNGHError } from "./RNGHError"
import { InteractionManager } from './InteractionManager';
import { RNGHLogger } from './RNGHLogger';
import { TapGestureHandler } from './TapGestureHandler';
import { PanGestureHandler } from "./PanGestureHandler"
import { NativeViewGestureHandler } from "./NativeViewGestureHandler"


export class GestureHandlerFactory {
  private orchestrator: GestureHandlerOrchestrator
  private interactionManager = new InteractionManager()
  private factoryLogger: RNGHLogger

  constructor(private logger: RNGHLogger, private scrollLocker: ScrollLocker) {
    this.factoryLogger = logger.cloneWithPrefix("Factory")
    this.orchestrator = new GestureHandlerOrchestrator(logger.cloneWithPrefix("Orchestrator"))
  }

  create(handlerName: string, handlerTag: number): GestureHandler {
    this.factoryLogger.info(`create ${handlerName} with handlerTag: ${handlerTag}`)
    const deps: GestureHandlerDependencies = {
      tracker: new PointerTracker(),
      orchestrator: this.orchestrator,
      handlerTag,
      interactionManager: this.interactionManager,
      logger: this.logger.cloneWithPrefix("GestureHandler"),
      scrollLocker: this.scrollLocker,
    }
    switch (handlerName) {
      case "TapGestureHandler":
        return new TapGestureHandler(deps)
      case "PanGestureHandler":
        return new PanGestureHandler(deps)
      case "NativeViewGestureHandler":
        return new NativeViewGestureHandler(deps)
      default:
        const msg = `Unknown handler type: ${handlerName}`
        this.factoryLogger.info(msg)
        throw new RNGHError(msg)
    }
  }
}