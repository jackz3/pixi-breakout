export default class Input {
  keyDownSet = new Set()
  keyPressedSet = new Set()
  onKeyPressed = (key:string) => {}
  constructor () {
    this.init()
  }
  init () {
    document.addEventListener('keydown', (e) => {
      this.keyDownSet.add(e.key)
    })
    document.addEventListener('keyup', e => {
      this.keyDownSet.delete(e.key)
      if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'Escape'].includes(e.key)) {
        this.keyPressedSet.add(e.key)
        this.onKeyPressed(e.key)
      }
    })
    document.addEventListener('keypress', (e) => {
      this.keyPressedSet.add(e.key)
      this.onKeyPressed(e.key)
    })
  }
}
