import Brick from './Brick'
import { rand } from './utils'

export default class LevelMaker {
  static createMap (container:PIXI.Container, level:number = 1):Brick[] {
    // const NONE = 1
    // const SINGLE_PYRAMID = 2
    // const MULTI_PYRAMID = 3

    // const SOLID = 1
    // const ALTERNATE = 2
    // const SKIP = 3
    // const NONE = 4

    const bricks = []

    let numRows = rand(4, 1)
    let numCols = rand(6, 7)
    numCols = numCols % 2 === 0 ? numCols + 1 : numCols

    const highestTier = Math.min(3, Math.floor(level / 5))
    const highestColor = Math.min(5, level % 5 + 3)

    for (let y = 0; y < numRows; y++) {
      //-- whether we want to enable skipping for this row
      let skipPattern = Math.random() > 0.5 ? true : false
      // -- whether we want to enable alternating colors for this row
      let alternatePattern = Math.random() > 0.5 ? true : false
      // -- choose two colors to alternate between
      let alternateColor1 = rand(highestColor, 1)
      let alternateColor2 = rand(highestColor, 1)
      let alternateTier1 = rand(highestTier)
      let alternateTier2 = rand(highestTier)
      //-- used only when we want to skip a block, for skip pattern
      let skipFlag = Math.random() > 0.5 ? true : false
      //-- used only when we want to alternate a block, for alternate pattern
      let alternateFlag = Math.random() > 0.5 ? true : false
      // -- solid color we'll use if we're not alternating
      let solidColor = rand(highestColor, 1)
      let solidTier = rand(highestTier)
      for (let x = 0; x < numCols; x++) {
        // -- if skipping is turned on and we're on a skip iteration...
        if (skipPattern && skipFlag) {
                // -- turn skipping off for the next iteration
          skipFlag = !skipFlag
                // -- Lua doesn't have a continue statement, so this is the workaround
          continue
        } else {
                // -- flip the flag to true on an iteration we don't use it
          skipFlag = !skipFlag
        }
        const b = new Brick(x * 32 +8 + (13 - numCols) * 16, (y + 1) * 16)
        // -- if we're alternating, figure out which color/tier we're on
        if (alternatePattern && alternateFlag) {
          b.color = alternateColor1
          b.tier = alternateTier1
          alternateFlag = !alternateFlag
        } else {
          b.color = alternateColor2
          b.tier = alternateTier2
          alternateFlag = !alternateFlag
        }

        // -- if not alternating and we made it here, use the solid color/tier
        if (!alternatePattern) {
            b.color = solidColor
            b.tier = solidTier
        }
        // b.setTexture(container)
        bricks.push(b)
      }
    }

    if (bricks.length == 0) {
      return LevelMaker.createMap(container, level)
    }
    return bricks
  }
}