export enum State {
  /** This is the initial state of each handler and it goes into this state after it's done recognizing a gesture. */
  UNDETERMINED,

  /** A handler received some touches but for some reason didn't recognize them. For example, if a finger travels more
   * distance than a defined maxDist property allows, then the handler won't become active but will fail instead.
   * Afterwards, it's state will be reset to UNDETERMINED. */
  FAILED,

  /** Handler has started receiving touch stream but hasn't yet received enough data to either fail or activate. */
  BEGAN,

  /** The gesture recognizer has received a signal (possibly new touches or a command from the touch system controller)
   * resulting in the cancellation of a continuous gesture. The gesture's state will become CANCELLED until it is
   * finally reset to the initial state, UNDETERMINED. */
  CANCELLED,

  /** Handler has recognized a gesture. It will become and stay in the ACTIVE state until the gesture finishes
   * (e.g. when user lifts the finger) or gets cancelled by the touch system. Under normal circumstances the state will
   * then turn into END. In the case that a gesture is cancelled by the touch system, its state would then become
   * CANCELLED. Learn about discrete and continuous handlers here to understand how long a handler can be kept in the
   * ACTIVE state.
   * */
  ACTIVE,

  /** The gesture recognizer has received touches signalling the end of a gesture. Its state will become END until it is
   * reset to UNDETERMINED.
   * */
  END,
}

export function getStateName(state: State): string {
  switch (state) {
    case State.UNDETERMINED:
      return "UNDETERMINED"
    case State.FAILED:
      return "FAILED"
    case State.BEGAN:
      return "BEGAN"
    case State.CANCELLED:
      return "CANCELLED"
    case State.ACTIVE:
      return "ACTIVE"
    case State.END:
      return "END"
  }
}