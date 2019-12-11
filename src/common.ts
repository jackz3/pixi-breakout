import {VirtualScreen} from './constants'
import global from './global'
import {Sprite, Text, Container} from 'pixi.js'

export class CommonRender {
  static health = -1
  static score = -1
  static hearts:Sprite[] = []
  static misHearts:Sprite[] = []
  static scoreTxt = new Text('', {fill: 'white', fontFamily: ['Arial'], fontSize: 9})
  static rednerHeart (container:Container, health:number) {
    if (health === CommonRender.health) {
      return
    }
    CommonRender.clear(container)
    CommonRender.health = health
    let healthX = VirtualScreen.width - 100
    for (let i = 0; i < health; i++) {
      const heartSprite = Sprite.from(global.frames.hearts1)
      heartSprite.x = healthX
      heartSprite.y = 4
      CommonRender.hearts.push(heartSprite)
      container.addChild(heartSprite)
      healthX += 11
    }

    for (let i = 0; i < (3 - health); i++) {
      const misHeartSprite = Sprite.from(global.frames.hearts2)
      misHeartSprite.x = healthX
      misHeartSprite.y = 4
      CommonRender.misHearts.push(misHeartSprite)
      container.addChild(misHeartSprite)
      healthX += 11
    }
  }
  static clear (container:Container) {
    container.removeChild(...CommonRender.hearts)
    container.removeChild(...CommonRender.misHearts)
    CommonRender.hearts = []
    CommonRender.misHearts = []
  }
  static renderScore (container:Container, score:number) {
    if (CommonRender.score === score) {
      return
    }
    CommonRender.score = score
    CommonRender.scoreTxt.text = `Score: ${score}`
    if (!container.children.includes(CommonRender.scoreTxt)) {
      CommonRender.scoreTxt.x = VirtualScreen.width - 60
      CommonRender.scoreTxt.y = 2
      container.addChild(CommonRender.scoreTxt)
    }
  }
}