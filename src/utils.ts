import * as PIXI from 'pixi.js'

export function center (displable:PIXI.Sprite, width:number, height:number, startX:number = 0, startY:number = 0) {
  hCenter(displable, width, startX)
  displable.y = (height - displable.height) / 2 + startY
}
export function hCenter (displable:PIXI.Sprite, width:number, startX:number = 0) {
  displable.x = (width - displable.width) / 2 + startX
}

export interface IPixiSpriteSheet {
  frames:{[key:string]: {frame:any, rotated: boolean, trimmed: boolean, spriteSourceSize: any, sourceSize: any}},
  meta:any
}

export async function getSpriteFrames (baseTexture:PIXI.BaseTexture, sheetObj:IPixiSpriteSheet) {
  const spriteSheet = new PIXI.Spritesheet(baseTexture, sheetObj)
  return new Promise((resolve, reject) => spriteSheet.parse(frames => {
    resolve(frames)
  }))
}

export function generateQuads (baseWidth:number, baseHeight:number, tilewidth:number, tileheight:number, name:string = '', sheetObj?:IPixiSpriteSheet) {
  if (!sheetObj) {
    sheetObj ={
      frames: {},
      meta: {}
    }
  }
  const sheetWidth = baseWidth / tilewidth
  const sheetHeight = baseHeight / tileheight

  let sheetCounter = 1
  for (let y = 0; y < sheetHeight; y++) {
    for (let x = 0; x < sheetWidth; x++) {
      sheetObj.frames[`${name}${sheetCounter}`] = {
        "frame": {"x": x * tilewidth,"y": y * tileheight,"w": tilewidth,"h": tileheight},
        "rotated": false,
        "trimmed": false,
        "spriteSourceSize": {"x":0,"y":0,"w": tilewidth,"h": tileheight},
        "sourceSize": {"w": tilewidth,"h": tileheight}
      }
      sheetCounter++
    }
  }
  return sheetObj
}

export class StateStack {
  states:BaseState[] = [new BaseState()]
  update(delta:number) {
    this.states[this.states.length - 1].update(delta)
  }
  processAI(params:any, delta:number) {
    this.states[this.states.length - 1].processAI(params, delta)
  }
  render() {
    this.states.forEach(state => state.render())
  }
  clear() {
    this.states = []
  }
  push (state:BaseState) {
    this.states.push(state)
    state.enter()
  }
  pop() {
    const state = this.states.pop()
    if (state) {
      state.exit()
    }
  }
}

export class StateMachine {
  empty: BaseState
  current: BaseState
  constructor (public states:{[state: string]: Function} = {}) {
    this.empty = new BaseState()
    this.current = this.empty
  }
  change (stateName:string, params?:any) {
    if (this.states[stateName]) {
      this.current.exit()
      this.current = this.states[stateName]()
      this.current.enter(params)
    }
  }
  update (delta:number) {
    this.current.update(delta)
  }
  render () {
    this.current.render()
  }
  processAI (params:any, delta:number) {
    this.current.processAI(params, delta)
  }
}

export class BaseState {
  enter (params?:any) {}
  exit() {}
  update (delta:number) {}
  render() {}
  processAI(params:any, delta:number) {}
}

export function aabbCollision (x1:number, y1:number, w1:number, h1:number, x2:number, y2:number,w2:number, h2:number): boolean {
  return x1 < x2+w2 &&
         x2 < x1+w1 &&
         y1 < y2+h2 &&
         y2 < y1+h1
}

export function rand(to:number, from:number = 0) {
  return Math.floor(Math.random() * to) + from
}

export function rgbToNum (r:number, g:number, b:number) {
  return (r << 16) + (g << 8) + b
}
