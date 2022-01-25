#pragma once
#include <string>
#include <map>
#include <array>
#include <iostream>
#include "raylib.h"

namespace breakout {
class GlobalSound {
public:
  GlobalSound() {
    std::string sources[][2] = {
      {"paddle-hit","../assets/paddle_hit.wav"},
      {"confirm", "../assets/confirm.wav"},
      {"wall-hit","../assets/wall_hit.wav"}
    };
    int len = sizeof sources / sizeof sources[0];
    InitAudioDevice();
    init(sources, len);
  }
  void init(const std::string sources[][2], int len) {
    for (int i = 0; i < len; i++) {
      sounds_[sources[i][0]] = LoadSound((sources[i][1]).c_str());
    }
  }
  void play(std::string name) {
    Sound sound = sounds_[name];
    PlaySoundMulti(sound);
  }
  void unload() {
    StopSoundMulti();       // We must stop the buffer pool before unloading
    for (auto& sound : sounds_) {
      UnloadSound(sound.second);
    }
  }
private:
  std::map<std::string, Sound> sounds_ = {};

};
} // namespace breakout end