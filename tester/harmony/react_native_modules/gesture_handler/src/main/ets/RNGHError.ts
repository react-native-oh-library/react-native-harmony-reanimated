export class RNGHError extends Error {
  constructor(message: string) {
    super("rnoh-gesture-handler: " + message)
  }
}