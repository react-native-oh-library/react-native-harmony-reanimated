import { RNOHContext } from "rnoh/ts"

export interface RNGHLogger {
  info(msg: string): void

  cloneWithPrefix(prefix: string): RNGHLogger
}

export class StandardRNGHLogger implements RNGHLogger {
  constructor(private rnohLogger: RNOHContext["logger"], private prefix: string) {
  }

  info(msg: string) {
    this.rnohLogger.info(`${this.prefix}::${msg}`)
  }

  cloneWithPrefix(prefix: string) {
    return new StandardRNGHLogger(this.rnohLogger, `${this.prefix}::${prefix}`)
  }
}

export class FakeRNGHLogger implements RNGHLogger {
  info(msg: string) {
  }

  cloneWithPrefix(prefix: string) {
    return new FakeRNGHLogger()
  }
}