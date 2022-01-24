import global from '../global'
import Paddle from '../Paddle'
import Brick from '../Brick'
import Ball from '../Ball'
import {BaseState, hCenter} from '../utils'
import { VirtualScreen } from '../constants'
import { Text } from 'pixi.js'
import {CommonRender} from '../common'

export default class PlayState extends BaseState {
  paddle!:Paddle
  ball!:Ball
  bricks:Brick[] = []
  health = 0
  score = 0
  level = 0
  paused = false
  highScores = []
  recoverPoints = 0
  pauseTxt = new Text('PAUSED', {fill: 'white', fontFamily: ['Arial'], fontSize: 30, fontWeight: 'bold'})
  constructor (public container:PIXI.Container) {
    super()
    this.pauseTxt.visible = false
    this.pauseTxt.y = VirtualScreen.height / 2 - 16
    hCenter(this.pauseTxt, VirtualScreen.width)
  }
  enter (params:any) {
    this.paddle = params.paddle
    this.bricks = params.bricks
    this.health = params.health
    this.score = params.score
    this.highScores = params.highScores
    this.level = params.level
    this.ball = params.ball
    this.recoverPoints = params.recoverPoints
    this.ball.init()

    this.bricks.forEach(b => b.setTexture(this.container))
    this.container.addChild(this.paddle.sprite, this.ball.sprite, ...this.bricks.map(x => x.sprite), this.pauseTxt)
  }
  exit () {
    this.container.removeChildren()
  }
  update (delta:number) {
    if (this.paused) {
      if (global.input.keyPressedSet.has(' ')) {
        this.paused = false
        this.pauseTxt.visible = false
        global.sounds['pause'].play()
      } else
        return
    } else if (global.input.keyPressedSet.has(' ')) {
        this.paused = true
        this.pauseTxt.visible = true
        global.sounds['pause'].play()
        return
    }

    this.paddle.update(delta)
    this.ball.update(delta)

    if (this.ball.collides(this.paddle)) {
      this.ball.y -= 8
      this.ball.dy = -this.ball.dy

      if (this.ball.x < this.paddle.x + (this.paddle.width / 2) && this.paddle.dx < 0) {
        this.ball.dx = -1 + -(0.15 * (this.paddle.x + this.paddle.width / 2 - this.ball.x))
      } else if (this.ball.x > this.paddle.x + (this.paddle.width / 2) && this.paddle.dx > 0) {
        this.ball.dx = 1 + (0.15 * Math.abs(this.paddle.x + this.paddle.width / 2 - this.ball.x))
      }
      global.sounds['paddle-hit'].play()
    }

    let collidlBrick = false
    this.bricks.forEach((brick) => {
      brick.update(delta)
      if (brick.inPlay && this.ball.collides(brick)) {
        this.score += (brick.tier * 200 + brick.color * 25)
        brick.hit(this.container)
        if (this.checkVictory()) {
          global.sounds['victory'].play()
          global.stateMachine.change('victory', {
            level: this.level,
            paddle: this.paddle,
            health: this.health,
            score: this.score,
            ball: this.ball
          })
        }
        if (collidlBrick) {
          return
        }
        if (this.ball.x + 2 < brick.x && this.ball.dx > 0) {
          this.ball.dx = -this.ball.dx
          this.ball.x = brick.x - 8
        } else if (this.ball.x + 6 > brick.x + brick.width && this.ball.dx < 0) {
          this.ball.dx = -this.ball.dx
          this.ball.x = brick.x + 32
        } else if (this.ball.y < brick.y) {
          this.ball.dy = -this.ball.dy
          this.ball.y = brick.y - 8
        } else {
          this.ball.dy = -this.ball.dy
          this.ball.y = brick.y + 16
        }
        this.ball.dy = this.ball.dy * 1.02
        collidlBrick = true
      }
    })

    if (this.ball.y >= VirtualScreen.height) {
        this.health -= 1
        global.sounds['hurt'].play()

        if (this.health == 0) {
          global.stateMachine.change('game-over', {
            score: this.score,
            highScores: this.highScores
          })
          this.container.removeChildren()
        } else {
          global.stateMachine.change('serve', {
              paddle: this.paddle,
              bricks: this.bricks,
              health: this.health,
              score: this.score,
              highScores: this.highScores,
              level: this.level,
              recoverPoints: this.recoverPoints
            })
          }
        }
    // if love.keyboard.wasPressed('escape') then
    //     love.event.quit()
    // end
  }
  render () {
    this.bricks.forEach(brick => {
      brick.render()
    })
    this.paddle.render()
    this.ball.render()

    CommonRender.renderScore(this.container, this.score)
    CommonRender.rednerHeart(this.container, this.health)
  }
  checkVictory ():boolean {
    return !this.bricks.some(brick => {
      if (brick.inPlay) {
        return true
      }
    })
  }
}