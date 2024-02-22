import {RNPackage, TurboModuleContext, TurboModulesFactory} from 'rnoh/ts';
import type {TurboModule} from 'rnoh/ts';
import {ReanimatedModule} from './ReanimatedModule';
class ReanimatedTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === ReanimatedModule.NAME) {
      return new ReanimatedModule(this.ctx);
    }
    return null;
  }
  hasTurboModule(name: string): boolean {
    return name === 'ReanimatedModule';
  }
}
export class ReanimatedPackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new ReanimatedTurboModulesFactory(ctx);
  }
}