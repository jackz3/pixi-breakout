import {BaseState, center, hCenter} from '../utils'
import global from '../global'
import {Text} from 'pixi.js'
import { VirtualScreen } from '../constants'
import Paddle from '../Paddle'
import LevelMaker from '../LevelMaker'

export default class StartState extends BaseState {
  highScores:any[] = []
  highlighted = 1
  titleTxt = new Text('BREAKOUT', {fill: 'white', fontFamily: ['Arial'], fontSize: 30, fontWeight: 'bold'})
  startTxt = new Text('START', {fill: 'white', fontFamily: ['Arial'], fontSize: 12})
  highScoresTxt = new Text('HIGH SCORES', {fill: 'white', fontFamily: ['Arial'], fontSize: 12})
  constructor(public container:PIXI.Container) {
    super()
    this.titleTxt.y = VirtualScreen.height / 3
    hCenter(this.titleTxt, VirtualScreen.width)
    this.startTxt.y = VirtualScreen.height / 2 + 70
    hCenter(this.startTxt, VirtualScreen.width)
    this.highScoresTxt.y = VirtualScreen.height / 2 + 90
    hCenter(this.highScoresTxt, VirtualScreen.width)
    container.addChild(this.titleTxt)
    container.addChild(this.startTxt)
    container.addChild(this.highScoresTxt)
    this.renderMenu()
  }
  enter (params:any) {
    this.highScores = params.highScores
  }
  exit () {
    this.container.removeChildren()
  }
  update (delta:number) {
    if (global.input.keyPressedSet.has('ArrowUp') || global.input.keyPressedSet.has('ArrowDown')) {
      this.highlighted = this.highlighted === 1 ? 2 : 1
      global.sounds['paddle-hit'].play()
      this.renderMenu()
    }
    if (global.input.keyPressedSet.has('Enter')) {
      global.sounds['confirm'].play()

      if (this.highlighted === 1) {
        global.stateMachine.change('paddle-select', {highScores: this.highScores})
      } else {
        global.stateMachine.change('high-scores', {highScores: this.highScores})
      }
    }
    if (global.input.keyPressedSet.has('escape')) {
      console.log('quit')
    }
  }
  renderMenu () {
    if (this.highlighted === 1) {
      this.startTxt.style =  {...this.startTxt.style, fill: '#67ffff'}
      this.highScoresTxt.style = {...this.highScoresTxt.style, fill: '#ffffff'}
    } else {
      this.highScoresTxt.style = {...this.startTxt.style, fill: '#67ffff'}
      this.startTxt.style = {...this.highScoresTxt.style, fill: '#ffffff'}
    }
  }
}