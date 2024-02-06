import { RNInstance } from "rnoh/ts"
import { ScrollLocker } from "./GestureHandler"

export class RNOHScrollLocker implements ScrollLocker {
  public constructor(private rnInstance: RNInstance) {
  }

  public lockScrollContainingViewTag(viewTag: number) {
    return this.rnInstance.blockComponentsGestures(viewTag)
  }
}