import { DescriptorRegistry, ComponentManagerRegistry, Tag } from "rnoh/ts"
import { View } from "./View"

export class ViewRegistry {
  constructor(private descriptorRegistry: DescriptorRegistry, private componentManagerRegistry: ComponentManagerRegistry) {
  }

  public getViewByTag(viewTag: Tag) {
    return this.createView(viewTag)
  }

  private createView(tag: Tag): View {
    return new View(this.descriptorRegistry, tag)
  }

  public getTouchableViewsAt(pos: {
    x: number,
    y: number
  }, rootTag: Tag): View[] {
    const rootView = this.createView(rootTag)
    const results: View[] = []
    for (const view of this.getTouchableViewsAtPosInView(pos, rootView)) {
      results.push(view)
    }
    return results
  }

  private getTouchableViewsAtPosInView(pos: {
    x: number,
    y: number
  }, view: View) {
    if (!view.isPositionInBounds(pos))
      return [];
    const results: View[] = []
    results.push(view)
    for (const child of view.getChildren()) {
      for (const result of this.getTouchableViewsAtPosInView(pos, child)) {
        results.push(result)
      }
    }
    return results
  }
}