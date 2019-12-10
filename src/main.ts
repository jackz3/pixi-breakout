import * as PIXI from 'pixi.js'
import {Screen, VirtualScreen} from './constants'
import {generateQuads, getSpriteFrames} from './utils'
import global from './global'
import StartState from './states/StartState'
import PlayState from './states/PlayState'
import ServeState from './states/ServeState'
import GameOverState from './states/GameOverState'
import VictoryState from './states/VictoryState'

const app = new PIXI.Application({
  ...Screen
})
document.body.appendChild(app.view)
PIXI.settings.SCALE_MODE = PIXI.SCALE_MODES.NEAREST
app.stage.scale.set(Screen.width / VirtualScreen.width, Screen.height / VirtualScreen.height)

const bgSprite = PIXI.Sprite.from('../assets/background.png')
bgSprite.width = VirtualScreen.width + 1
bgSprite.height = VirtualScreen.height + 2 
app.stage.addChild(bgSprite)

const heartsTexture = PIXI.BaseTexture.from('../assets/hearts.png')
const heartsSheetObj = generateQuads(20, 9, 10, 9, 'hearts')
const heartsFrames = getSpriteFrames(heartsTexture, heartsSheetObj)

const breakoutTexture = PIXI.BaseTexture.from('../assets/breakout.png')
const breakoutSheetObj = generateQuads(192, 128, 16, 16, 'entities')
generateQuadsPaddles(breakoutSheetObj)
generateQuadsBalls(breakoutSheetObj)
const breakoutSheet = new PIXI.Spritesheet(breakoutTexture, breakoutSheetObj)
breakoutSheet.parse(async frames => {
  global.frames = {...global.frames, ...frames}

  const allFrames = await Promise.all([heartsFrames])
  allFrames.reduce((res, x) => {
    return Object.assign(res, x)
  }, global.frames)

  const container = new PIXI.Container()
  app.stage.addChild(container)
  global.stateMachine.states = {
    start: () => new StartState(container),
    play: () => new PlayState(container),
    serve: () => new ServeState(container),
    'game-over': () => new GameOverState(container),
    victory: () => new VictoryState(container),
    // 'paddle-select': () => new
    //  ['high-scores'] = function() return HighScoreState() end,
        // ['enter-high-score'] = function() return EnterHighScoreState() end,
  }
  global.stateMachine.change('start', {highScores: []})
  app.ticker.add((delta) => {
    global.stateMachine.update(delta)
    global.stateMachine.render()
    global.input.keyPressedSet.clear()
  })
})

function generateQuadsPaddles (spriteSheetObj:any) {
  let x = 0
  let y = 64

  let counter = 1

  for (let i = 0; i < 3; i++) {
    spriteSheetObj.frames['paddle' + counter] = {
      "frame": {"x": x,"y": y,"w": 32,"h": 16},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 32,"h": 16},
      "sourceSize": {"w": 32,"h": 16}
    }
    counter++
    spriteSheetObj.frames['paddle' + counter] = {
      "frame": {"x": x + 32,"y": y,"w": 64,"h": 16},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 64,"h": 16},
      "sourceSize": {"w": 64,"h": 16}
    }
    counter++
    spriteSheetObj.frames['paddle' + counter] = {
      "frame": {"x": x + 96,"y": y,"w": 96,"h": 16},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 96,"h": 16},
      "sourceSize": {"w": 96,"h": 16}
    }
    counter++
    spriteSheetObj.frames['paddle' + counter] = {
      "frame": {"x": x,"y": y + 16,"w": 128,"h": 16},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 126,"h": 16},
      "sourceSize": {"w": 128,"h": 16}
    }
    counter++
    x = 0
    y+= 32
  }
}
function generateQuadsBalls (spriteSheetObj:any) {
  let x = 96
  let y = 48

  let counter = 1
  for (let i = 0; i < 4; i++) {
    spriteSheetObj.frames['balls' + counter] = {
      "frame": {"x": x,"y": y,"w": 8,"h": 8},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 8,"h": 8},
      "sourceSize": {"w": 8,"h": 8}
    }
    x += 8
    counter++
  }

  x = 96
  y = 56

  for (let i = 0; i < 3; i++) {
    spriteSheetObj.frames['balls' + counter] = {
      "frame": {"x": x,"y": y,"w": 8,"h": 8},
      "rotated": false,
      "trimmed": false,
      "spriteSourceSize": {"x":0,"y":0,"w": 8,"h": 8},
      "sourceSize": {"w": 8,"h": 8}
    }
    x += 8
    counter++
  }
}