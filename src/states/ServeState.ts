import {BaseState, center, rand} from '../utils'
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
  msgTxt = new Text('Press Enter to serve!', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  constructor (public levelContainer:PIXI.Container) {
    super()
    center(this.msgTxt, VirtualScreen.width, VirtualScreen.height / 2)
  }
  enter (params:{paddle:Paddle, bricks:Brick[], health:number, score:number}) {
    this.paddle = params.paddle
    this.bricks = params.bricks
    this.health = params.health
    this.score = params.score

    this.ball = new Ball(this.levelContainer, Math.ceil(Math.random() * 7))

    this.levelContainer.addChild(this.msgTxt)
    // this.levelContainer.visible = true
  }
  exit () {
    this.levelContainer.removeChild(this.msgTxt)
    // this.msgTxt.visible = false
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
        ball: this.ball
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