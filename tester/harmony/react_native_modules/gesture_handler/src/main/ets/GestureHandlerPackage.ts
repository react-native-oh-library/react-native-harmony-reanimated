import {RNPackage, TurboModuleContext, TurboModulesFactory} from 'rnoh/ts';
import type {TurboModule} from 'rnoh/ts';
import {RNGestureHandlerModule} from './RNGestureHandlerModule';

class GestureHandlerTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === RNGestureHandlerModule.NAME) {
      return new RNGestureHandlerModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'RNGestureHandlerModule';
  }
}

export class GestureHandlerPackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new GestureHandlerTurboModulesFactory(ctx);
  }
}
