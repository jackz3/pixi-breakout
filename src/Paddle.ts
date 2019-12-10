import global from './global'
import { PADDLE_SPEED, VirtualScreen } from './constants'
import {Sprite} from 'pixi.js'

export default class Paddle {
  x = VirtualScreen.width / 2 - 32
  y = VirtualScreen.height - 32
  dx = 0
  width = 64
  height = 16
  size = 2
  sprite:Sprite
  constructor (public container:PIXI.Container, public skin:number) {
    this.sprite = new Sprite()
    this.sprite.texture = global.frames[`paddle${this.size + 4 * (this.skin - 1)}`]// as PIXI.Texture
    container.addChild(this.sprite)
  }
  // addTo (container?:PIXI.Container) {
  //   if (container) {
  //     this.container = container
  //   }
  //   if (!this.added) {
  //     this.container!.addChild(this.sprite!)
  //     this.added = true
  //   }
  // }
  update (delta:number) {
    if (global.input.keyDownSet.has('ArrowLeft')) {
      this.dx = -PADDLE_SPEED
    } else if (global.input.keyDownSet.has('ArrowRight')) {
      this.dx = PADDLE_SPEED
    } else
      this.dx = 0
    
    if (this.dx < 0) {
      this.x = Math.max(0, this.x + this.dx * delta)
    } else {
      this.x = Math.min(VirtualScreen.width - this.width, this.x + this.dx * delta)
    }
  }
  render () {
    this.sprite.x = this.x
    this.sprite.y = this.y
    // ove.graphics.draw(gTextures['main'], gFrames['paddles'][self.size + 4 * (self.skin - 1)], self.x, self.y)
  }
}