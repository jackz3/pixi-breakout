import {BaseState} from '../utils'
import global from '../global'

export class GameOverState extends BaseState {
  score:number = 0
  gameoverTxt = new PIXI.Text('GAME OVER', {fill: 'white', fontFamily: ['Arial'], fontSize: 36})
  scoreTxt = new PIXI.Text('Final Score: ', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  msgTxt = new PIXI.Text('Press Enter!', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  constructor (public container:PIXI.Container) {
    super()
    center(this.gameoverTxt, VirtualScreen.width, VirtualScreen.height / 3)
    center(this.scoreTxt, VirtualScreen.width, VirtualScreen.height /2)
    center(this.msgTxt, VirtualScreen.width, VirtualScreen.height * 0.75)
  }
  enter (params:{score:number}) {
    this.score = params.score
    this.scoreTxt.text = `Final Score: ${this.score}`
    this.container.addChild(this.gameoverTxt)
    this.container.addChild(this.scoreTxt)
    this.container.addChild(this.msgTxt)
    this.container.visible = true
  }
  exit () {
    this.container.removeChild(this.gameoverTxt, this.scoreTxt, this.msgTxt)
    this.container.visible = false
  }
  update (delta:number) {
    if (keyPressedSet.has('Enter')) {
      global.stateMachine.change('start')
    }

    // if love.keyboard.wasPressed('escape') then
    //     love.event.quit()
    // end
  }
}