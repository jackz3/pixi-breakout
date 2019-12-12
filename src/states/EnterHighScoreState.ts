import {BaseState, hCenter} from '../utils'
import global from '../global'
import Storage from '../Storage'
import {Text, Container} from 'pixi.js'
import { VirtualScreen } from '../constants'

const chars = [65, 65, 65]
export default class EnterHighScoreState extends BaseState {
// -- individual chars of our string
  // -- char we're currently changing
  highlightedChar = 1
  highScores:any[] = []
  score = 0
  scoreIndex = 0
  infoTxt = new Text('', {fontSize: 14, fill: '#FFFFFF'})
  msgTxt = new Text('Press Enter to confirm!', {fontSize: 10})
  constructor (public container:Container) {
    super()
    this.infoTxt.y = 30
    this.msgTxt.y = VirtualScreen.height - 18
    hCenter(this.msgTxt, VirtualScreen.width)
  }
  enter (params:any) {
    this.highScores = params.highScores
    this.score = params.score
    this.scoreIndex = params.scoreIndex
    this.infoTxt.text = 'Your score: ' + this.score
    hCenter(this.infoTxt, VirtualScreen.width)
    this.container.addChild(this.infoTxt, this.msgTxt)
  }
  exit () {

  }
  update (delta:number) {
    if (global.input.keyPressedSet.has('Enter')) {
      // -- update scores table
      let name = String.fromCharCode(chars[1]) + String.fromCharCode(chars[2]) + String.fromCharCode(chars[3])
      // -- go backwards through high scores table till this score, shifting scores
      for (let i = 9; i >= this.scoreIndex; i--) {
        this.highScores[i] = {
          name: this.highScores[i].name,
          score: this.highScores[i].score
        }
      }

      this.highScores[this.scoreIndex].name = name
      this.highScores[this.scoreIndex].score = this.score

      // -- write scores to file
      let scoresStr = ''

      for (let i = 0; i < 10; i++) {
        scoresStr = scoresStr .. this.highScores[i].name .. '\n'
        scoresStr = scoresStr .. tostring(this.highScores[i].score) .. '\n'
      }

      new Storage().save(scoresStr)
        // love.filesystem.write('breakout.lst', scoresStr)

      global.stateMachine.change('high-scores', {
        highScores: this.highScores
      })
    }
    // -- scroll through character slots
    if (global.input.keyPressedSet.has('ArrowLeft') && this.highlightedChar > 1) {
      this.highlightedChar -= 1
      global.sounds['select'].play()
    } else if (global.input.keyPressedSet.has('ArrowRight') && this.highlightedChar < 3) {
      this.highlightedChar ++
      global.sounds['select'].play()
    }
    // -- scroll through characters
    if (global.input.keyPressedSet.has('ArrowUp')) {
      chars[this.highlightedChar] = chars[this.highlightedChar] + 1
      if (chars[this.highlightedChar] > 90) {
        chars[this.highlightedChar] = 65
      }
    } else if (global.input.keyPressedSet.has('ArrowDown')) {
      chars[this.highlightedChar] = chars[this.highlightedChar] - 1
      if (chars[this.highlightedChar] < 65) {
        chars[this.highlightedChar] = 90
      }
    }
  }

  render () {
    love.graphics.setFont(gFonts['large'])
    // -- render all three characters of the name
    if (this.highlightedChar === 1) {
      love.graphics.setColor(103, 255, 255, 255)
    }
    love.graphics.print(string.char(chars[1]), VIRTUAL_WIDTH / 2 - 28, VIRTUAL_HEIGHT / 2)
    love.graphics.setColor(255, 255, 255, 255)

    if highlightedChar == 2 then
        love.graphics.setColor(103, 255, 255, 255)
    end
    love.graphics.print(string.char(chars[2]), VIRTUAL_WIDTH / 2 - 6, VIRTUAL_HEIGHT / 2)
    love.graphics.setColor(255, 255, 255, 255)

    if highlightedChar == 3 then
        love.graphics.setColor(103, 255, 255, 255)
    end
    love.graphics.print(string.char(chars[3]), VIRTUAL_WIDTH / 2 + 20, VIRTUAL_HEIGHT / 2)
    love.graphics.setColor(255, 255, 255, 255)

    // love.graphics.setFont(gFonts['small'])
    // love.graphics.printf('Press Enter to confirm!', 0, VIRTUAL_HEIGHT - 18,        VIRTUAL_WIDTH, 'center')
  }
}
