import global from '../global'
import { VirtualScreen } from '../constants'
import {BaseState, hCenter} from '../utils'

export default class GameOverState extends BaseState {
  score:number = 0
  highScores = []
  gameoverTxt = new PIXI.Text('GAME OVER', {fill: 'white', fontFamily: ['Arial'], fontSize: 32, fontWeight: 'bold'})
  scoreTxt = new PIXI.Text('Final Score: ', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  msgTxt = new PIXI.Text('Press Enter!', {fill: 'white', fontFamily: ['Arial'], fontSize: 24})
  constructor (public container:PIXI.Container) {
    super()
    this.gameoverTxt.y = VirtualScreen.height / 3
    hCenter(this.gameoverTxt, VirtualScreen.width)
    this.msgTxt.y = VirtualScreen.height * 3 / 4
    hCenter(this.msgTxt, VirtualScreen.width)
  }
  enter (params:{score:number}) {
    this.score = params.score
    this.scoreTxt.text = `Final Score: ${this.score}`
    this.scoreTxt.y = VirtualScreen.height /2
    hCenter(this.scoreTxt, VirtualScreen.width)
    this.container.addChild(this.gameoverTxt, this.scoreTxt, this.msgTxt)
  }
  exit () {
    this.container.removeChildren()
  }
  update (delta:number) {
    if (global.input.keyPressedSet.has('Enter')) {
      // -- see if score is higher than any in the high scores table
      let highScore = false
      // -- keep track of what high score ours overwrites, if any
      // local scoreIndex = 11

      //   for i = 10, 1, -1 do
      //       local score = self.highScores[i].score or 0
      //       if self.score > score then
      //           highScoreIndex = i
      //           highScore = true
      //       end
      //   end

      //   if highScore then
      //       gSounds['high-score']:play()
      //       gStateMachine:change('enter-high-score', {
      //           highScores = self.highScores,
      //           score = self.score,
      //           scoreIndex = highScoreIndex
      //       }) 
      //   else 
      global.stateMachine.change('start')
    }
    // if love.keyboard.wasPressed('escape') then
    //     love.event.quit()
    // end
  }
}