import { GestureHandler, GestureConfig, GestureHandlerDependencies, DEFAULT_TOUCH_SLOP } from "./GestureHandler"
import { AdaptedEvent } from "./Event"
import { State } from "./State"
import { Vector2D } from "./Vector2D"

const DEFAULT_MIN_DIST_SQ = DEFAULT_TOUCH_SLOP * DEFAULT_TOUCH_SLOP;

type PanGestureHandlerConfig = GestureConfig

export class PanGestureHandler extends GestureHandler<PanGestureHandlerConfig> {
  private startPos = new Vector2D();
  private offset = new Vector2D()
  private lastPos = new Vector2D();
  private velocity = new Vector2D();
  private activationTimeout = 0;

  private get failOffsetXStart() {
    if (this.config.failOffsetXStart === undefined
      && this.config.failOffsetXEnd === undefined)
      return undefined
    return this.config.failOffsetXStart ?? Number.MIN_SAFE_INTEGER
  }

  private get failOffsetXEnd() {
    if (this.config.failOffsetXStart === undefined
      && this.config.failOffsetXEnd === undefined)
      return undefined
    return this.config.failOffsetXEnd ?? Number.MAX_SAFE_INTEGER
  }

  private get failOffsetYStart() {
    if (this.config.failOffsetYStart === undefined
      && this.config.failOffsetYEnd === undefined)
      return undefined
    return this.config.failOffsetYStart ?? Number.MIN_SAFE_INTEGER
  }

  private get failOffsetYEnd() {
    return this.config.failOffsetYEnd ?? Number.MAX_SAFE_INTEGER
  }

  private get activeOffsetXStart() {
    if (this.config.activeOffsetXStart === undefined
      && this.config.activeOffsetXEnd === undefined)
      return undefined
    return this.config.activeOffsetXStart ?? Number.MIN_SAFE_INTEGER
  }

  private get activeOffsetXEnd() {
    if (this.config.activeOffsetXStart === undefined
      && this.config.activeOffsetXEnd === undefined)
      return undefined
    return this.config.activeOffsetXEnd ?? Number.MAX_SAFE_INTEGER
  }

  private get activeOffsetYStart() {
    if (this.config.activeOffsetYStart === undefined
      && this.config.activeOffsetYEnd === undefined)
      return undefined
    return this.config.activeOffsetYStart ?? Number.MIN_SAFE_INTEGER
  }

  private get activeOffsetYEnd() {
    if (this.config.activeOffsetYStart === undefined
      && this.config.activeOffsetYEnd === undefined)
      return undefined
    return this.config.activeOffsetYEnd ?? Number.MAX_SAFE_INTEGER
  }

  private get minVelocityX() {
    return this.config.minVelocityX ?? this.config.minVelocity ?? Number.MAX_SAFE_INTEGER
  }

  private get minVelocityY() {
    return this.config.minVelocityY ?? this.config.minVelocityY ?? Number.MAX_SAFE_INTEGER
  }

  private minVelocitySq = Number.MAX_SAFE_INTEGER

  private get minPointers() {
    return this.config.minPointers ?? 1
  }

  private unlockScrolls: (() => void) | undefined

  public constructor(deps: GestureHandlerDependencies) {
    super({ ...deps, logger: deps.logger.cloneWithPrefix("PanGestureHandler") })
  }

  public onPointerDown(e) {
    this.tracker.addToTracker(e);
    super.onPointerDown(e);
    this.lastPos = this.tracker.getLastAvgPos()
    this.startPos = this.lastPos.clone()
    this.tryBegin(e);
    this.tryActivating();
  }

  private tryActivating(): void {
    if (this.currentState === State.BEGAN) {
      if (this.shouldFail()) {
        this.fail();
      } else if (this.shouldActivate()) {
        this.activate();
      }
    }
  }

  private shouldFail(): boolean {
    const {x: dx, y: dy} = this.getTranslation().value;
    const distanceSq = dx * dx + dy * dy;
    if (this.activateAfterLongPress > 0 && distanceSq > DEFAULT_MIN_DIST_SQ) {
      this.clearActivationTimeout();
      return true;
    }
    if (this.failOffsetXStart !== Number.MIN_SAFE_INTEGER && dx < this.failOffsetXStart) {
      return true;
    }
    if (this.failOffsetXEnd !== Number.MAX_SAFE_INTEGER && dx > this.failOffsetXEnd) {
      return true;
    }
    if (this.failOffsetYStart !== Number.MIN_SAFE_INTEGER && dy < this.failOffsetYStart) {
      return true;
    }
    return (this.failOffsetYEnd !== Number.MAX_SAFE_INTEGER && dy > this.failOffsetYEnd);
  }

  private getTranslation() {
    return this.lastPos.clone().subtract(this.startPos).add(this.offset)
  }

  private shouldActivate(): boolean {
    const {x: dx, y: dy} = this.getTranslation().value;
    if (this.activeOffsetXStart !== Number.MAX_SAFE_INTEGER && dx < this.activeOffsetXStart
    ) {
      return true;
    }
    if (this.activeOffsetXEnd !== Number.MIN_SAFE_INTEGER && dx > this.activeOffsetXEnd) {
      return true;
    }
    if (this.activeOffsetYStart !== Number.MAX_SAFE_INTEGER && dy < this.activeOffsetYStart) {
      return true;
    }
    if (this.activeOffsetYEnd !== Number.MIN_SAFE_INTEGER && dy > this.activeOffsetYEnd) {
      return true;
    }
    const distanceSq: number = dx * dx + dy * dy;
    if (this.minDistSq !== Number.MAX_SAFE_INTEGER && distanceSq >= this.minDistSq) {
      return true;
    }
    const {x: vx, y: vy} = this.velocity
    if (
      this.minVelocityX !== Number.MAX_SAFE_INTEGER &&
        ((this.minVelocityX < 0 && vx <= this.minVelocityX) ||
          (this.minVelocityX >= 0 && this.minVelocityX <= vx))
    ) {
      return true;
    }
    if (
      this.minVelocityY !== Number.MAX_SAFE_INTEGER &&
        ((this.minVelocityY < 0 && vy <= this.minVelocityY) ||
          (this.minVelocityY >= 0 && this.minVelocityY <= vy))
    ) {
      return true;
    }
    const velocitySq: number = vx * vx + vy * vy;
    return (
      this.minVelocitySq !== Number.MAX_SAFE_INTEGER &&
        velocitySq >= this.minVelocitySq
    );
  }

  private clearActivationTimeout(): void {
    clearTimeout(this.activationTimeout);
  }

  private tryBegin(e: AdaptedEvent): void {
    if (
      this.currentState === State.UNDETERMINED &&
        this.tracker.getTrackedPointersCount() >= this.minPointers
    ) {
      this.resetProgress();
      this.offset = new Vector2D();
      this.velocity = new Vector2D();

      this.begin();

      if (this.activateAfterLongPress > 0) {
        this.activationTimeout = setTimeout(() => {
          this.activate();
        }, this.activateAfterLongPress);
      }
    } else {
      this.velocity = this.tracker.getVelocity(e.pointerId)
    }
  }

  public getDefaultConfig() {
    return {}
  }

  private get activateAfterLongPress() {
    return this.config.activateAfterLongPress ?? 0
  }

  private get minDistSq() {
    if (this.config.minDist !== undefined) {
      return this.config.minDist * this.config.minDist;
    } else if (this.hasCustomActivationCriteria()) {
      return Number.MAX_SAFE_INTEGER;
    }
    return DEFAULT_MIN_DIST_SQ
  }

  private hasCustomActivationCriteria() {
    const criterias: (keyof PanGestureHandlerConfig)[] = [
      'activeOffsetXStart',
      'activeOffsetXEnd',
      'failOffsetXStart',
      'failOffsetXEnd',
      'activeOffsetYStart',
      'activeOffsetYEnd',
      'failOffsetYStart',
      'failOffsetYEnd',
      'minVelocityX',
      'minVelocityY',
    ]
    for (const key in this.config) {
      if (criterias.indexOf(key as keyof PanGestureHandlerConfig) >= 0) {
        return true
      }
    }
    return false
  }

  public onAdditionalPointerAdd(event: AdaptedEvent): void {
    this.tracker.addToTracker(event);
    super.onAdditionalPointerAdd(event);
    this.tryBegin(event);
    this.offset.add(this.lastPos).subtract(this.startPos)
    this.lastPos = this.tracker.getLastAvgPos()
    this.startPos = this.lastPos.clone()
    if (this.tracker.getTrackedPointersCount() > (this.config.maxPointers ?? 10)) {
      if (this.currentState === State.ACTIVE) {
        this.cancel();
      } else {
        this.fail();
      }
    } else {
      this.tryActivating();
    }
  }

  public onPointerUp(event: AdaptedEvent): void {
    super.onPointerUp(event);
    if (this.currentState === State.ACTIVE) {
      this.lastPos = this.tracker.getLastAvgPos();
    }
    this.tracker.removeFromTracker(event.pointerId);
    if (this.currentState === State.ACTIVE) {
      this.end();
    } else {
      this.resetProgress();
      this.fail();
    }
  }

  public onAdditionalPointerRemove(event: AdaptedEvent): void {
    super.onAdditionalPointerRemove(event);
    this.tracker.removeFromTracker(event.pointerId);
    this.offset.add(this.lastPos).subtract(this.startPos)
    this.lastPos = this.tracker.getLastAvgPos()
    this.startPos = this.lastPos.clone()
    if (
      !(
        this.currentState === State.ACTIVE &&
          this.tracker.getTrackedPointersCount() < this.minPointers
      )
    ) {
      this.tryActivating();
    }
  }

  public onPointerMove(event: AdaptedEvent): void {
    this.tracker.track(event);
    this.lastPos = this.tracker.getLastAvgPos()
    this.velocity = this.tracker.getVelocity(event.pointerId)
    this.tryActivating();
    super.onPointerMove(event);
  }

  public onPointerOutOfBounds(event: AdaptedEvent): void {
    if (this.shouldCancelWhenOutside) {
      return;
    }
    this.tracker.track(event);
    this.lastPos = this.tracker.getLastAvgPos()
    this.velocity = this.tracker.getVelocity(event.pointerId)
    this.tryActivating();
    if (this.currentState === State.ACTIVE) {
      super.onPointerOutOfBounds(event);
    }
  }

  protected transformNativeEvent() {
    const rect = this.view.getBoundingRect();
    const translation = this.getTranslation()
    return {
      translationX: translation.x,
      translationY: translation.y,
      absoluteX: this.tracker.getLastAvgX(),
      absoluteY: this.tracker.getLastAvgY(),
      velocityX: this.velocity.x,
      velocityY: this.velocity.y,
      x: this.tracker.getLastAvgX() - rect.x,
      y: this.tracker.getLastAvgY() - rect.y,
    };
  }

  protected stateDidChange(newState: State, oldState: State) {
    super.stateDidChange(newState, oldState)
    if (newState === State.BEGAN) {
      this.unlockScrolls = this.scrollLocker.lockScrollContainingViewTag(this.view.getTag())
    } else if (newState !== State.ACTIVE) {
      this.unlockScrolls?.()
    }
  }
}