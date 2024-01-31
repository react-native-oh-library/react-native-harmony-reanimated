export class Vector2D {
  constructor(
    private val: {
      x: number;
      y: number;
    } = {x: 0, y: 0},
  ) {}

  get x() {
    return this.val.x;
  }

  get y() {
    return this.val.y;
  }

  get value() {
    return {...this.val};
  }

  public clone() {
    return new Vector2D({...this.val});
  }

  public subtract(vec: Vector2D) {
    this.val.x -= vec.x;
    this.val.y -= vec.y;
    return this;
  }

  public add(vec: Vector2D) {
    this.val.x += vec.x;
    this.val.y += vec.y;
    return this;
  }
}
