import { ComponentBuilderContext, RNOHLogger } from 'rnoh';
import {
  RNApp,
  RNAbility,
  RNOHErrorDialog,
  AnyJSBundleProvider,
  MetroJSBundleProvider,
  ResourceJSBundleProvider,
  TraceJSBundleProviderDecorator
} from 'rnoh'
import font from '@ohos.font';
import { createRNPackages } from '../RNPackagesFactory'
import { RNGestureHandlerButton, RNGestureHandlerRootView } from 'rnoh-gesture-handler';


@Builder
export function CustomComponentBuilder(ctx: ComponentBuilderContext) {
  if (ctx.componentName === RNGestureHandlerRootView.NAME) {
    RNGestureHandlerRootView({
      tag: ctx.tag,
      ctx: ctx.rnohContext,
      buildCustomComponent: CustomComponentBuilder
    })
  } else if (ctx.descriptor.type === RNGestureHandlerButton.DESCRIPTOR_TYPE) {
    RNGestureHandlerButton({
      tag: ctx.descriptor.tag,
      ctx: ctx.rnohContext,
      buildCustomComponent: CustomComponentBuilder
    })
  }
}

/**
 * If you want to use custom fonts, you need to register them here.
 * We should support react-native-asset to handle registering fonts automatically.
 */
const fonts: font.FontOptions[] = [
  {
    familyName: 'Pacifico-Regular',
    familySrc: '/assets/fonts/Pacifico-Regular.ttf'
  },
  {
    familyName: 'StintUltraCondensed-Regular',
    familySrc: '/assets/fonts/StintUltraCondensed-Regular.ttf'
  }
]

@Entry
@Component
struct Index {
  @StorageLink('RNAbility') rnAbility: RNAbility | undefined = undefined
  @State shouldShow: boolean = false
  private logger!: RNOHLogger

  aboutToAppear() {
    this.logger = this.rnAbility!.getLogger().clone("Index")
    const stopTracing = this.logger.clone("aboutToAppear").startTracing()
    for (const customFont of fonts) {
      font.registerFont(customFont)
    }

    this.shouldShow = true
    stopTracing()
  }

  onBackPress(): boolean | undefined {
    // NOTE: this is required since `Ability`'s `onBackPressed` function always
    // terminates or puts the app in the background, but we want Ark to ignore it completely
    // when handled by RN
    return this.rnAbility?.onBackPress();
  }

  build() {
    Column() {
      if (this.rnAbility && this.shouldShow) {
        if (this.rnAbility.isDebugModeEnabled()) {
          RNOHErrorDialog()
        }
        RNApp({
          rnInstanceConfig: { createRNPackages },
          initialProps: { "foo": "bar" } as Record<string, string>,
          appKey: "tester",
          buildCustomComponent: CustomComponentBuilder,
          onSetUp: (rnInstance) => {
            rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
          },
          jsBundleProvider: new TraceJSBundleProviderDecorator(
            new AnyJSBundleProvider([
              new MetroJSBundleProvider(),
              new ResourceJSBundleProvider(this.rnAbility.context.resourceManager, 'hermes_bundle.hbc'),
              new ResourceJSBundleProvider(this.rnAbility.context.resourceManager, 'bundle.harmony.js')
              ]),
            this.rnAbility.getLogger()),
        })
      }
    }
    .height('100%')
    .width('100%')
  }
}
