import global from './global'
import {Sprite, Container} from 'pixi.js'
import {VirtualScreen} from './constants'
import {aabbCollision} from './utils'

export default class Ball {
  width = 8
  height = 8
  dy = 0
  dx = 0
  x = 0
  y = 0
  sprite: Sprite
  constructor (/*public container:Container, */public skin:number) {
    this.sprite = new Sprite()
    this.sprite.texture = global.frames[`balls${this.skin}`]
    // container.addChild(this.sprite)
  }
  init () {
    this.dx = Math.random() * 6 - 3
    this.dy = Math.random() * 0.3 - 1

    this.x = VirtualScreen.width / 2 - 4
    this.y = VirtualScreen.height - 42
  }
  reset() {
    this.x = VirtualScreen.width / 2 - 2
    this.y = VirtualScreen.height / 2 - 2
    this.dx = 0
    this.dy = 0
  }
  collides (target:any) {
    return aabbCollision(this.x, this.y, this.width, this.height, target.x, target.y, target.width, target.height)
  }
  update (delta:number) {
    this.x += this.dx * delta
    this.y += this.dy * delta

    if (this.x <= 0) {
      this.x = 0
      this.dx = -this.dx
      global.sounds['wall-hit'].play()
    }

    if (this.x >= VirtualScreen.width - 8) {
        this.x = VirtualScreen.width - 8
        this.dx = -this.dx
        global.sounds['wall-hit'].play()
    }

    if (this.y <= 0) {
      this.y = 0
      this.dy = -this.dy
      global.sounds['wall-hit'].play()
    }
  }
  render () {
    this.sprite.x = this.x
    this.sprite.y = this.y
  }
}