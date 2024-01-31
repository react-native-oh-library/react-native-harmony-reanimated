import type {RNPackageContext, RNPackage} from 'rnoh/ts';
import {ReanimatedPackage} from 'rnoh-reanimated/ts';
import {GestureHandlerPackage } from "rnoh-gesture-handler/ts";


export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [new ReanimatedPackage(ctx), new GestureHandlerPackage(ctx)];
}
