import {TurboModule, TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  installTurboModule(): void;
}

export default TurboModuleRegistry.get<Spec>(
  'ReanimatedModule',
) as Spec;