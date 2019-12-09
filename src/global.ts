import * as PIXI from 'pixi.js'
import {Howl} from 'howler'
import Input from './input'
import {StateMachine} from './utils'

interface Global {
  stateMachine: StateMachine,
  input:Input,
  sounds: {
    [key:string]:Howl
  },
  frames: {
    [key: string]: PIXI.Texture
  },
  [key: string]: any,
}
const global:Global = {
  stateMachine: new StateMachine(),
  input: new Input(),
  sounds: {
    'paddle-hit': new Howl({src: ['../assets/paddle_hit.wav']}),
    'score': new Howl({src: ['../assets/score.wav']}),
    'wall-hit': new Howl({src: ['../assets/wall_hit.wav']}),
    'confirm': new Howl({src: ['../assets/confirm.wav']}),
    'select': new Howl({src: ['../assets/select.wav']}),
    'no-select': new Howl({src: ['../assets/no-select.wav']}),
    'brick-hit-1': new Howl({src: ['../assets/brick-hit-1.wav']}),
    'brick-hit-2': new Howl({src: ['../assets/brick-hit-2.wav']}),
    'hurt': new Howl({src: ['../assets/hurt.wav']}),
    'victory': new Howl({src: ['../assets/victory.wav']}),
    'recover': new Howl({src: ['../assets/recover.wav']}),
    'high-score': new Howl({src: ['../assets/high_score.wav']}),
    'pause': new Howl({src: ['../assets/pause.wav']}),
    'music': new Howl({src: ['../assets/music.wav'], loop: true})
  },
  frames: {}
}
export default global