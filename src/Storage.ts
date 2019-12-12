import localforage from 'localforage'

export default class Storage {
  async load () {
    let scores:any[] = await localforage.getItem('scores')
    if (!scores) {
      scores = []
      for (let i = 10 ; i > 0; i--) {
        scores.push({
          name: 'abc',
          score: i * 1000
        })
      }
      await this.save(scores)
    }
    return scores
  }
  async save (scores:any) {
    await localforage.setItem('scores', scores)
  }
}