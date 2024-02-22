import {
  RNApp,
  RNOHErrorDialog,
  ComponentBuilderContext,
  RNAbility,
  MetroJSBundleProvider
} from 'rnoh'
import { RNGestureHandlerRootView, RNGestureHandlerButton, GestureHandlerPackage } from "rnoh-gesture-handler"
import {ReanimatedPackage} from 'rnoh-reanimated';

@Builder
function CustomComponentBuilder(ctx: ComponentBuilderContext) {
  if (ctx.componentName === RNGestureHandlerRootView.NAME) {
    RNGestureHandlerRootView({
      tag: ctx.tag,
      ctx: ctx.rnComponentContext,
      buildCustomComponent: CustomComponentBuilder
    })
  } else if (ctx.componentName === RNGestureHandlerButton.NAME) {
    RNGestureHandlerButton({
      tag: ctx.tag,
      ctx: ctx.rnComponentContext,
      buildCustomComponent: CustomComponentBuilder
    })
  }
}

const wrappedCustomRNComponentBuilder = wrapBuilder(CustomComponentBuilder)

@Entry
@Component
struct Index {
  @StorageLink('RNAbility') rnAbility: RNAbility | undefined = undefined

  build() {
    Column() {
      if (this.rnAbility) {
        RNOHErrorDialog()
        RNApp({
          appKey: "tester",
          rnInstanceConfig: { createRNPackages: (ctx) => [
            new GestureHandlerPackage(ctx),
            new ReanimatedPackage(ctx)
          ]},
          buildCustomComponent: CustomComponentBuilder,
          jsBundleProvider: new MetroJSBundleProvider(),
          wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
        })
      }
    }
    .height('100%')
    .width('100%')
  }
}
