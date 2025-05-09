// import { RNGestureHandlerModule } from '@react-native-oh-tpl/react-native-gesture-handler/ts';
import { RNOHLogger, Tag, TurboModule, TurboModuleContext } from "@rnoh/react-native-openharmony/ts";
import { window } from '@kit.ArkUI';
import { AnimationManager } from './AnimationManager'

export class ReanimatedModule extends TurboModule {
  static NAME = "ReanimatedModule"
  private logger: RNOHLogger
  private keyBordHeight = 0;

  constructor(ctx:  TurboModuleContext) {
    super(ctx);
    this.logger = ctx.logger.clone("Reanimated");
  }
  public setViewProps(tag: Tag, props: Object) {
    this.ctx.descriptorRegistry.setAnimatedRawProps(tag, props);
  }

  public async subscribeKeyBordListeners() {
    const windowInstance = await window.getLastWindow(this.ctx.uiAbilityContext);
    windowInstance.on('keyboardHeightChange', async (height) => {
      let keyBordHeight = this.ctx.getUIContext().px2vp(height);
      const animationManager = new AnimationManager();
      animationManager.setUpdateCallback((value, status) => {
        this.ctx.rnInstance.postMessageToCpp("REANIMATED_KEY_BOARD_CHANGE",
          { keyboardHeight: value, status: status.valueOf() });
      });
      animationManager.startAnimation(this.keyBordHeight, keyBordHeight, 200);
      this.keyBordHeight = keyBordHeight;
    })
  }

  public async unsubscribeKeyBordListeners() {
    const windowInstance = await window.getLastWindow(this.ctx.uiAbilityContext);
    windowInstance.off('keyboardHeightChange', (v) => {
    })
  }
}