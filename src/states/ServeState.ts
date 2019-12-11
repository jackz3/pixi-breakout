import {BaseState, rand, hCenter} from '../utils'
import global from '../global'
import Paddle from '../Paddle'
import Brick from '../Brick'
import Ball from '../Ball'
import { VirtualScreen } from '../constants'
import { Text } from 'pixi.js'
import { CommonRender } from '../common'

export default class ServeState extends BaseState {
  paddle?:Paddle
  bricks:Brick[] = []
  health:number = 0
  score:number = 0
  ball?:Ball
  highScores = 0
  level = 0
  recoverPoints = 0
  TitleTxt = new Text('', {fill: 'white', fontSize: 30})
  msgTxt = new Text('Press Enter to serve!', {fill: 'white', fontFamily: ['Arial'], fontSize: 14})
  constructor (public levelContainer:PIXI.Container) {
    super()
    this.TitleTxt.y = VirtualScreen.height / 3
    this.msgTxt.y = VirtualScreen.height / 2
    hCenter(this.msgTxt, VirtualScreen.width)
  }
  enter (params:any) {
    this.paddle = params.paddle
    this.bricks = params.bricks
    this.health = params.health
    this.score = params.score
    this.highScores = params.highScores
    this.level = params.level
    this.recoverPoints = params.recoverPoints
    this.ball = new Ball(rand(7, 1))
    this.TitleTxt.text = `Level ${this.level}`
    hCenter(this.TitleTxt, VirtualScreen.width)
    this.levelContainer.addChild(this.paddle!.sprite, this.ball!.sprite, ...this.bricks.map(x => x.sprite), this.msgTxt, this.TitleTxt)
  }
  exit () {
    this.levelContainer.removeChildren()
  }
  update (delta:number) {
    this.ball!.x = 1
    if (this.paddle && this.ball) {
      this.paddle.update(delta)
      this.ball.x = this.paddle.x + (this.paddle.width / 2) - 4
      this.ball.y = this.paddle.y - 8
    }

    if (global.input.keyPressedSet.has('Enter')) {
      global.stateMachine.change('play', {
        paddle: this.paddle,
        bricks: this.bricks,
        health: this.health,
        score: this.score,
        ball: this.ball,
        highScores: this.highScores,
        level: this.level,
        recoverPoints: this.recoverPoints
      })
    }
    // if love.keyboard.wasPressed('escape') then
    //     love.event.quit()
    // end
  }
  render () {
    if (this.paddle && this.ball) {
      this.paddle.render()
      this.ball.render()
    }

    this.bricks.forEach(brick => brick.render())

    CommonRender.renderScore(this.levelContainer, this.score)
    CommonRender.rednerHeart(this.levelContainer, this.health)
  }
}