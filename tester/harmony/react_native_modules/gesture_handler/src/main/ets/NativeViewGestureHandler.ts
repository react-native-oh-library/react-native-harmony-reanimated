import { GestureHandler, GestureHandlerDependencies, DEFAULT_TOUCH_SLOP } from "./GestureHandler"
import { Vector2D } from './Vector2D';
import { State } from "./State"
import { AdaptedEvent } from './Event';

export class NativeViewGestureHandler extends GestureHandler {
  private minDistSq = DEFAULT_TOUCH_SLOP * DEFAULT_TOUCH_SLOP;

  protected startPos = new Vector2D()

  public canBeInterrupted() {
    return !(this.config.disallowInterruption ?? false)
  }

  constructor(deps: GestureHandlerDependencies) {
    super({ ...deps, logger: deps.logger.cloneWithPrefix("NativeViewGestureHandler") })
  }

  public getDefaultConfig() {
    return {}
  }

  public onPointerDown(e: AdaptedEvent) {
    this.tracker.addToTracker(e);
    super.onPointerDown(e);
    this.onNewPointer();
  }

  protected onNewPointer() {
    this.startPos = this.tracker.getLastAvgPos();
    if (this.currentState !== State.UNDETERMINED)
      return;
    this.begin();
    this.activate();
  }

  public onAdditionalPointerAdd(e: AdaptedEvent) {
    this.tracker.addToTracker(e);
    super.onPointerDown(e);
    this.onNewPointer();
  }

  public onPointerMove(e: AdaptedEvent): void {
    this.tracker.track(e);
    const {x: dx, y: dy} = this.startPos.clone().subtract(this.tracker.getLastAvgPos()).value
    const distSq = dx * dx + dy * dy;

    if (distSq >= this.minDistSq) {
      if (this.currentState === State.ACTIVE) {
        this.cancel();
      } else if (this.currentState === State.BEGAN) {
        this.activate();
      }
    }
  }

  public onPointerLeave(): void {
    // TODO: add this method to GestureHandler
    if (this.currentState === State.BEGAN || this.currentState === State.ACTIVE) {
      this.cancel();
    }
  }

  public onPointerUp(event: AdaptedEvent): void {
    super.onPointerUp(event);
    this.onAnyPointerUp(event);
  }

  private onAnyPointerUp(e: AdaptedEvent) {
    this.tracker.removeFromTracker(e.pointerId);
    if (this.tracker.getTrackedPointersCount() === 0) {
      if (this.currentState === State.ACTIVE) {
        this.end();
      } else {
        this.fail();
      }
    }
  }

  public onAdditionalPointerRemove(e: AdaptedEvent) {
    super.onAdditionalPointerRemove(e)
    this.onAnyPointerUp(e)
  }

  public shouldRecognizeSimultaneously(handler: GestureHandler): boolean {
    if (super.shouldRecognizeSimultaneously(handler)) {
      return true;
    }
    if (
      handler instanceof NativeViewGestureHandler &&
        handler.getState() === State.ACTIVE &&
        !handler.canBeInterrupted()
    ) {
      return false;
    }

    if (
      this.currentState === State.ACTIVE &&
        handler.getState() === State.ACTIVE &&
        this.canBeInterrupted()
    ) {
      return false;
    }

    return (
      this.currentState === State.ACTIVE &&
        this.canBeInterrupted() &&
        handler.getTag() > 0
    );
  }

  public shouldBeCancelledByOther(_handler: GestureHandler): boolean {
    return this.canBeInterrupted();
  }
}