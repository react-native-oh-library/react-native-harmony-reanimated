'use strict'

import * as Animated from 'react-native-reanimated/src/Animated';

export * from 'react-native-reanimated/src/reanimated2';
import ReanimatedModule from './ReanimatedModule';

global._REANIMATED_VERSION_JS = "3.6.0";
ReanimatedModule.installTurboModule();

export default Animated;
