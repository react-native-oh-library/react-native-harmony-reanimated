import { State } from "./State"
import { TouchEventType } from "./Event"

export interface GestureEventPayload {
  handlerTag: number;
  numberOfPointers: number;
  state: State;
}

export interface HandlerStateChangeEventPayload extends GestureEventPayload {
  oldState: State;
}

export type GestureUpdateEvent<GestureEventPayloadT = Record<string, unknown>> = GestureEventPayload
  & GestureEventPayloadT

export type GestureStateChangeEvent<GestureStateChangeEventPayloadT = Record<string, unknown>> = HandlerStateChangeEventPayload & GestureStateChangeEventPayloadT

export type TouchData = {
  id: number;
  x: number;
  y: number;
  absoluteX: number;
  absoluteY: number;
};

export type GestureTouchEvent = {
  handlerTag: number;
  numberOfTouches: number;
  state: State;
  eventType: TouchEventType;
  allTouches: TouchData[];
  changedTouches: TouchData[];
};