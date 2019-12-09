import global from './global'

const paletteColors:{[idx:string]:string} = {
  '1': '639cff',
  '2': '6bbe2f',
  '3': 'd95764',
  '4': 'd77bba',
  '5': 'd7f236'
}
export default class Brick {
  tier = 0
  color = 1
  width = 32
  height = 16
  inPlay = true
  sprite:PIXI.Sprite
  // pSystem?:ParticleSystem
  constructor (public container:PIXI.Container, public x:number, public y:number) {
    this.sprite = new PIXI.Sprite()
    // this.sprite.texture = global.frames[`bricks${1 + ((this.color - 1) * 4) + this.tier}`]
    container.addChild(this.sprite)
  }
  setTexture () {
    this.sprite.texture = global.frames[`bricks${1 + ((this.color - 1) * 4) + this.tier}`]
    // this.pSystem = new ParticleSystem(this.container, ['../assets/particle.png'], {
    //   "alpha": {
    //       "start": 55 * (this.tier + 1) / 255,
    //       "end": 0
    //   },
    //   "color": {
    //     "start": paletteColors[this.color],
    //     "end": paletteColors[this.color]
    //   }
    // })
  }
  hit () {
    // this.pSystem!.emit(this.x + 16, this.y + 8)
    global.sounds['brick-hit-2'].stop()
    global.sounds['brick-hit-2'].play()

    if (this.tier > 0) {
      if (this.color === 1) {
        this.tier -= 1
        this.color = 5
      } else {
        this.color -= 1
      }
    } else {
        // -- if we're in the first tier and the base color, remove brick from play
      if (this.color === 1) {
        this.inPlay = false
        this.sprite.visible = false
      } else {
        this.color -= 1
      }
    }
    this.setTexture()
    // -- play a second layer sound if the brick is destroyed
    if (!this.inPlay) {
      global.sounds['brick-hit-1'].stop()
      global.sounds['brick-hit-1'].play()
    }
  }
  update (delta:number) {
    // if (this.pSystem) {
    //   if (this.pSystem.play) {
    //     this.pSystem.update(delta)
    //   }
    // }
  }
  render () {
    this.sprite.x = this.x
    this.sprite.y = this.y
  }
}