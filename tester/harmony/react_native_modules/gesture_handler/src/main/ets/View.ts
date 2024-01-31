import { DescriptorRegistry, Descriptor, Tag } from "rnoh/ts"
import { Vector2D } from "./Vector2D"

export type BoundingBox = {
  x: number,
  y: number,
  width: number,
  height: number
}

export class View {
  constructor(private descriptorRegistry: DescriptorRegistry, private viewTag: number) {
  }

  public getChildren() {
    return this.getDescriptor().childrenTags.map(childrenTag => {
      return new View(this.descriptorRegistry, childrenTag)
    })
  }

  public getTag(): Tag {
    return this.viewTag
  }

  public isPositionInBounds({x, y}: {
    x: number,
    y: number
  }): boolean {
    const rect = this.getBoundingRect()
    return x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height);
  }

  public getBoundingRect(): BoundingBox {
    const d = this.getDescriptor()
    if (!d) {
      return { x: 0, y: 0, width: 0, height: 0 }
    }
    const offsetToAbsolutePosition = this.getOffsetToAbsolutePosition()
    return {
      x: d.layoutMetrics.frame.origin.x - offsetToAbsolutePosition.x,
      y: d.layoutMetrics.frame.origin.y - offsetToAbsolutePosition.y,
      width: d.layoutMetrics.frame.size.width,
      height: d.layoutMetrics.frame.size.height
    }
  }

  private getDescriptor() {
    return this.descriptorRegistry.getDescriptor(this.viewTag)
  }


  private getOffsetToAbsolutePosition(): Vector2D {
    const currentOffset = new Vector2D()
    let parentTag = this.getDescriptor()?.parentTag
    while (parentTag !== undefined) {
      const d = this.descriptorRegistry.getDescriptor(parentTag)
      currentOffset.add(this.extractScrollOffsetFromDescriptor(d))
      currentOffset.subtract(new Vector2D(d.layoutMetrics.frame.origin))
      parentTag = d.parentTag
    }
    return currentOffset
  }

  private extractScrollOffsetFromDescriptor(descriptor: Descriptor<any>) {
    if (descriptor.type !== "ScrollView") {
      return new Vector2D();
    }
    const scrollViewState: any = descriptor.state;
    return new Vector2D({ x: scrollViewState.contentOffsetX, y: scrollViewState.contentOffsetY })
  }
}