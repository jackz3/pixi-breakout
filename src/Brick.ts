import * as particles from 'pixi-particles'
import global from './global'
import { Sprite, Container, Texture } from 'pixi.js'

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
  sprite:Sprite
  pSystem?:ParticleSystem
  constructor (/*public container:PIXI.Container, */public x:number, public y:number) {
    this.sprite = new Sprite()
    this.sprite.texture = global.frames[`bricks${1 + ((this.color - 1) * 4) + this.tier}`]
    // container.addChild(this.sprite)
  }
  setTexture (container:Container) {
    this.sprite.texture = global.frames[`bricks${1 + ((this.color - 1) * 4) + this.tier}`]
    this.pSystem = new ParticleSystem(container, ['../assets/particle.png'], {
      "alpha": {
          "start": 55 * (this.tier + 1) / 255,
          "end": 0
      },
      "color": {
        "start": paletteColors[this.color],
        "end": paletteColors[this.color]
      }
    })
  }
  hit (container:Container) {
    this.pSystem!.emit(this.x + 16, this.y + 8)
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
    this.setTexture(container)
    // -- play a second layer sound if the brick is destroyed
    if (!this.inPlay) {
      global.sounds['brick-hit-1'].stop()
      global.sounds['brick-hit-1'].play()
    }
  }
  update (delta:number) {
    if (this.pSystem) {
      if (this.pSystem.play) {
        this.pSystem.update(delta)
      }
    }
  }
  render () {
    this.sprite.x = this.x
    this.sprite.y = this.y
  }
}

class ParticleSystem {
  emitter:particles.Emitter
  elapsed:number = 0
  play = false
  constructor (emitterContainer:Container, imgPaths:string[], cfg:any) {
    // const emitterContainer = new PIXI.Container()
    const art = []
    for(let i = 0; i < imgPaths.length; i++) {
      art.push(Texture.from(imgPaths[i]))
    }
		const config = {
        "alpha": {
          "start": 0.8,
          "end": 0.1
        },
        "scale": {
          "start": 5,
          "end": 1.2
        },
        "color": {
          "start": "fb1010",
          "end": "f5b830"
        },
        "speed": {
          "start": 700,
          "end": 10
        },
        "startRotation": {
          "min": 0,
          "max": 360
        },
        "rotationSpeed": {
          "min": 0,
          "max": 0
        },
        "lifetime": {
          "min": 0.5,
          "max": 1
        },
        "blendMode": "normal",
					"ease": [
						{
							"s": 0,
							"cp": 0.329,
							"e": 0.548
						},
						{
							"s": 0.548,
							"cp": 0.767,
							"e": 0.876
						},
						{
							"s": 0.876,
							"cp": 0.985,
							"e": 1
						}
					],
        "frequency": 0.008,
        "emitterLifetime": 0.31,
        "maxParticles": 128,
        "pos": {
          "x": 0,
          "y": 0
        },
        "addAtBack": false,
        "spawnType": "point",
        "spawnCircle": {
          "x": 0,
          "y": 0,
          "r": 10
        }
      }
    this.emitter = new particles.Emitter(
				emitterContainer,
				art,
				Object.assign(config, cfg)
    )
  }
  emit (x:number, y:number) {
    this.elapsed = Date.now()
    this.emitter.emit = true
    this.emitter.resetPositionTracking();
    this.emitter.updateOwnerPos(x, y)
    // this.update()
    this.play = true
  }
  update (delta:number) {
    this.emitter.update(delta / 60)
  }
  update1 () {
    requestAnimationFrame(this.update.bind(this))
    const now = Date.now()
    // The emitter requires the elapsed
    // number of seconds since the last update
    this.emitter.update((now - this.elapsed) * 0.001);
    this.elapsed = now
  }
}
