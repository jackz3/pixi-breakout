import {BaseState, center, hCenter} from '../utils'
import global from '../global'
import {Text} from 'pixi.js'
import { VirtualScreen } from '../constants'
import Paddle from '../Paddle'

export default class StartState extends BaseState {
  highlighted = 1
  titleTxt = new Text('BREAKOUT', {fill: 'white', fontFamily: ['Arial'], fontSize: 30, fontWeight: 'bold'})
  startTxt = new Text('START', {fill: 'white', fontFamily: ['Arial'], fontSize: 10})
  highScoresTxt = new Text('HIGH SCORES', {fill: 'white', fontFamily: ['Arial'], fontSize: 10})
  constructor(public container:PIXI.Container) {
    super()
    center(this.titleTxt, VirtualScreen.width, VirtualScreen.height / 3)
    center(this.startTxt, VirtualScreen.width, VirtualScreen.height / 2 - 70, 0, VirtualScreen.height /2 + 70)
    center(this.highScoresTxt, VirtualScreen.width, VirtualScreen.height / 2 - 90, 0, VirtualScreen.height /2 + 90)
    container.addChild(this.titleTxt)
    container.addChild(this.startTxt)
    container.addChild(this.highScoresTxt)
  }
  exit () {
    this.container.removeChildren()
  }
  update (delta:number) {
    if (global.input.keyPressedSet.has('ArrowUp') || global.input.keyPressedSet.has('ArrowDown')) {
      this.highlighted = this.highlighted === 1 ? 2 : 1
      global.sounds['paddle-hit'].play()
      if (this.highlighted === 1) {
        this.startTxt.style = {fill: '#67ffff', fontSize: 10}
        this.highScoresTxt.style = {fill: '#ffffff', fontSize: 10}
      } else {
        this.highScoresTxt.style = {fill: '#67ffff', fontSize: 10}
        this.startTxt.style = {fill: '#ffffff', fontSize: 10}
      }
    }
    if (global.input.keyPressedSet.has('Enter')) {
        global.sounds['confirm'].play()

        if (this.highlighted === 1) {
          global.stateMachine.change('serve', {
            paddle: new Paddle(this.container, 1),
            // bricks: LevelMaker.createMap(this.levelContainer),
            health: 3,
            score: 0,
            level: 1
          })
        }
    }
    if (global.input.keyPressedSet.has('escape')) {
      console.log('quit')
    }
  }
  render () {
  }
}