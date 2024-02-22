import { RNOHLogger, Tag, TurboModule, TurboModuleContext } from 'rnoh/ts';

export class ReanimatedModule extends TurboModule {
  static NAME = "ReanimatedModule"
  private logger: RNOHLogger
  constructor(ctx:  TurboModuleContext) {
    super(ctx);
    this.logger = ctx.logger.clone("Reanimated");
  }
  public setViewProps(tag: Tag, props: Object) {
    this.ctx.descriptorRegistry.setAnimatedRawProps(tag, props);
  }
}