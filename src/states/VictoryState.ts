import {BaseState} from '../utils'
import global from '../global'

export class VictoryState extends BaseState {
  level = 0
  score = 0
  paddle?:Paddle
  ball?:Ball
  health = 0
  levelTxt = new PIXI.Text('', {fill: 'white', fontFamily: ['Arial'], fontSize: 32})
  msgTxt = new PIXI.Text('', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  constructor (public container:PIXI.Container) {
    super()
    this.levelTxt.visible = false
    center(this.levelTxt, VirtualScreen.width, VirtualScreen.height / 4)
    container.addChild(this.levelTxt)
    this.msgTxt.visible = false
    center(this.msgTxt, VirtualScreen.width, VirtualScreen.height / 2)
    container.addChild(this.msgTxt)
  }
  enter (params:{level:number, score:number, paddle:Paddle, health:number, ball:Ball}) {
    this.level = params.level
    this.score = params.score
    this.paddle = params.paddle
    this.health = params.health
    this.ball = params.ball

    this.levelTxt.text = `Level ${this.level} complete`
    this.msgTxt.text = 'Press Enter to serve!'
    this.msgTxt.visible = true
    this.levelTxt.visible = true
    this.container.visible = true
  }
  update (delta:number) {
    if (this.paddle && this.ball) {
      this.paddle!.update(delta)

      this.ball.x = this.paddle.x + (this.paddle.width / 2) - 4
      this.ball.y = this.paddle.y - 8
    }

    if (keyPressedSet.has('Enter')) {
      global.stateMachine.change('serve', {
            level: this.level + 1,
            bricks: LevelMaker.createMap(this.container, this.level + 1),
            paddle: this.paddle,
            health: this.health,
            score: this.score
        })
    }
  }
  render () {
    HeartRender.rednerHeart(this.container, this.health)
    HeartRender.renderScore(this.container, this.score)
  }
}