import {BaseState, hCenter, rgbToNum} from '../utils'
import global from '../global'
import Paddle from '../Paddle'
import LevelMaker from '../LevelMaker'
import { Text, Container, Sprite } from 'pixi.js'
import { VirtualScreen } from '../constants'

export default class PaddleSelectState extends BaseState {
  highScores = []
  currentPaddle = 1
  titleTxt = new Text('Select your paddle with left and right!', {fontSize: '15', fill: '#ffffff'})
  msgTxt = new Text('(Press Enter to continue!)', {fontSize: 10, fill: '#ffffff'})
  leftArrow = Sprite.from(global.frames.arrows1)
  rightArrow = Sprite.from(global.frames.arrows2)
  paddle = Sprite.from(global.frames[`paddle${2 + 4 * (this.currentPaddle - 1)}`])
  constructor (public container:Container) {
    super()
    this.titleTxt.y = VirtualScreen.height / 4
    hCenter(this.titleTxt, VirtualScreen.width)
    this.msgTxt.y = VirtualScreen.height / 3
    hCenter(this.msgTxt, VirtualScreen.width)
    this.leftArrow.x = VirtualScreen.width / 4 - 24,
    this.leftArrow.y = VirtualScreen.height * 2 / 3
    this.rightArrow.x = VirtualScreen.width - VirtualScreen.width / 4,
    this.rightArrow.y = VirtualScreen.height * 2 / 3
    this.paddle.x = VirtualScreen.width / 2 - 32,
    this.paddle.y = VirtualScreen.height * 2 / 3
    container.addChild(this.titleTxt, this.msgTxt, this.leftArrow, this.rightArrow, this.paddle)
  }
  enter (params:any) {
    this.highScores = params.highScores
  }
  exit () {
    this.container.removeChildren()
  }
  update (dt:number) {
    if (global.input.keyPressedSet.has('ArrowLeft')) {
      if (this.currentPaddle === 1) {
        global.sounds['no-select'].play()
      } else {
        global.sounds['select'].play()
        this.currentPaddle -= 1
      }
    } else if (global.input.keyPressedSet.has('ArrowRight')) {
      if (this.currentPaddle === 4) {
        global.sounds['no-select'].play()
      } else {
        global.sounds['select'].play()
        this.currentPaddle++
      }
    }
    // -- select paddle and move on to the serve state, passing in the selection
    if (global.input.keyPressedSet.has('Enter')) {
      global.sounds['confirm'].play()
      global.stateMachine.change('serve', {
          paddle: new Paddle(this.currentPaddle),
          bricks: LevelMaker.createMap(this.container, 32),
          health: 3,
          score: 0,
          highScores: this.highScores,
          level: 32,
          recoverPoints: 5000
      })
    }
    // if (global.input.keyPressedSet.has('escape') then
        // love.event.quit()
  }
  renderPaddle () {

  }
  render () {
    // -- left arrow; should render normally if we're higher than 1, else
    // -- in a shadowy form to let us know we're as far left as we can go
    if (this.currentPaddle == 1) {
        // -- tint; give it a dark gray with half opacity
        // love.graphics.setColor(40, 40, 40, 128)
      this.leftArrow.tint = rgbToNum(40, 40, 40)
      this.leftArrow.alpha = 0.5
    } else {
      this.leftArrow.tint = 0xFFFFFF
      this.leftArrow.alpha = 1
    }
    // love.graphics.draw(gTextures['arrows'], gFrames['arrows'][1], VIRTUAL_WIDTH / 4 - 24, VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)

    // -- reset drawing color to full white for proper rendering
    // love.graphics.setColor(255, 255, 255, 255)

    // -- right arrow; should render normally if we're less than 4, else
    // -- in a shadowy form to let us know we're as far right as we can go
    if (this.currentPaddle === 4) {
        // -- tint; give it a dark gray with half opacity
        // love.graphics.setColor(40, 40, 40, 128)
      this.rightArrow.alpha = 0.5
    } else {
      this.rightArrow.alpha = 1
    }
    // love.graphics.draw(gTextures['arrows'], gFrames['arrows'][2], VIRTUAL_WIDTH - VIRTUAL_WIDTH / 4,        VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)
    // -- reset drawing color to full white for proper rendering
    // love.graphics.setColor(255, 255, 255, 255)
    // -- draw the paddle itthis, based on which we have selected
    // love.graphics.draw(gTextures['main'], gFrames['paddles'][2 + 4 * (this.currentPaddle - 1)], VIRTUAL_WIDTH / 2 - 32, VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)
    this.paddle.texture = global.frames[`paddle${2 + 4 * (this.currentPaddle - 1)}`]
  }
}
