import {BaseState, hCenter} from '../utils'
import global from '../global'
import {Text, Container} from 'pixi.js'
import { VirtualScreen } from '../constants'

export default class HighScoreState extends BaseState {
  highScores:any[] = []
  titleTxt = new Text('High Scores', {fontSize: '30'})
  msgTxt = new Text('Press Escape to return to the main menu!', {fontSize: '14'})
  constructor (public container:Container) {
    super()
    this.titleTxt.y = 20
    this.msgTxt.y = VirtualScreen.height - 18
    hCenter(this.titleTxt, VirtualScreen.width)
    hCenter(this.msgTxt, VirtualScreen.width)
  }
  enter (params:any) {
    this.highScores = params.highScores
    this.container.addChild(this.titleTxt, this.msgTxt)
  }
  update (delta:number) {
    // -- return to the start screen if we press escape
    if (global.input.keyPressedSet.has('Escape')) {
        global.sounds['wall-hit'].play()
        global.stateMachine.change('start', {
            highScores: this.highScores
        })
    }
  }
  render() {
    // love.graphics.setFont(gFonts['medium'])
    // -- iterate over all high score indices in our high scores table
    for (let i = 0; i < 10; i++) {
        let name = this.highScores[i].name || '---'
        let score = this.highScores[i].score || '---'
        // -- score number (1-10)
        const line = i + 1
        const noTxt = new Text(`${line}.`, {fontSize: 14, wordWrap: true, wordWrapWidth: 50})
        noTxt.x = VirtualScreen.width / 4
        noTxt.y = 60 + line * 13
        // love.graphics.printf(tostring(i) .. '.', VIRTUAL_WIDTH / 4,             60 + i * 13, 50, 'left')
        // -- score name
        const nameTxt = new Text(name, {fontSize: 14, wordWrap: true, wordWrapWidth: 50, align: 'right'})
        nameTxt.x = VirtualScreen.width / 4 + 38
        nameTxt.y = 60 + line * 13
        // love.graphics.printf(name, VIRTUAL_WIDTH / 4 + 38,             60 + i * 13, 50, 'right')
        // -- score itself
        const scoreTxt = new Text(score, {fontSize: 14, wordWrap: true, wordWrapWidth: 100, align: 'right'})
        scoreTxt.x = VirtualScreen.width / 2
        scoreTxt.y = 60 + line * 13
        // love.graphics.printf(tostring(score), VIRTUAL_WIDTH / 2,            60 + i * 13, 100, 'right')
        this.container.addChild(noTxt, nameTxt, scoreTxt)
    }
  }
    // love.graphics.setFont(gFonts['small'])
    // love.graphics.printf("Press Escape to return to the main menu!",
        // 0, VIRTUAL_HEIGHT - 18, VIRTUAL_WIDTH, 'center')
}
