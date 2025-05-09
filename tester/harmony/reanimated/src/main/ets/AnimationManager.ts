export class AnimationManager {
  private updateValue: number = 0;
  private animationId: number = 0;
  private duration: number = 200;
  private initialValue: number = 0;
  private targetValue: number = 1;
  private updateCallback?: (value: number, status: KeyboardState) => void;
  private status: KeyboardState = KeyboardState.UNKNOWN;

  setUpdateCallback(callback: (value: number, status: KeyboardState) => void): void {
    this.updateCallback = callback;
  }

  startAnimation(startValue: number, endValue: number, duration: number): void {
    if (this.animationId) {
      this.stopAnimation();
    }
    this.initialValue = startValue;
    this.targetValue = endValue;
    this.duration = duration;
    let startTime = Date.now();
    this.status = endValue > startValue ? KeyboardState.OPENING : KeyboardState.CLOSING;

    this.animationId = setInterval(() => {
      const elapsedTime = Date.now() - startTime;
      const progress = Math.min(elapsedTime / this.duration, 1);

      this.updateValue = this.initialValue + progress * (this.targetValue - this.initialValue);
      if (progress >= 1) {
        this.status = endValue > startValue ? KeyboardState.OPEN : KeyboardState.CLOSED;
      }
      if (this.updateCallback) {
        this.updateCallback(this.updateValue, this.status);
      }
      if (progress >= 1) {
        this.stopAnimation();
      }
    }, 8);
  }

  // 停止动画
  stopAnimation(): void {
    if (this.animationId) {
      clearInterval(this.animationId);
      this.animationId = 0;
    }
    this.status = KeyboardState.UNKNOWN;
  }
}

export enum KeyboardState {
  UNKNOWN = 0,
  OPENING = 1,
  OPEN = 2,
  CLOSING = 3,
  CLOSED = 4,
}