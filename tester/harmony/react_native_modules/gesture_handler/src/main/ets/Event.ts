export interface HitSlop {
  left?: number;
  right?: number;
  top?: number;
  bottom?: number;
  horizontal?: number;
  vertical?: number;
  width?: number;
  height?: number;
}

export const Directions = {
  RIGHT: 1,
  LEFT: 2,
  UP: 4,
  DOWN: 8,
} as const;

export enum PointerType {
  NONE = 'none',
  MOUSE = 'mouse',
  TOUCH = 'touch',
  PEN = 'pen',
}

export enum EventType {
  DOWN,
  ADDITIONAL_POINTER_DOWN,
  UP,
  ADDITIONAL_POINTER_UP,
  MOVE,
  ENTER,
  OUT,
  CANCEL,
}

export type Touch = {id: number, x: number, y: number, absoluteX: number, absoluteY: number}

export enum TouchEventType {
  UNDETERMINED = 0,
  DOWN = 1,
  MOVE = 2,
  UP = 3,
  CANCELLED = 4,
}

export interface AdaptedEvent {
  x: number;
  y: number;
  offsetX: number;
  offsetY: number;
  pointerId: number;
  eventType: EventType;
  pointerType: PointerType;
  buttons: number;
  time: number;
  allTouches?: Touch[];
  changedTouches?: Touch[];
  touchEventType?: TouchEventType;
}

export interface TrackerElement {
  lastX: number;
  lastY: number;
  timeStamp: number;
  velocityX: number;
  velocityY: number;
}