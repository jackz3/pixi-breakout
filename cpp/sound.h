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
      {"wall-hit","../assets/wall_hit.wav"},
      {"select", "../assets/select.wav"},
      {"no-select", "../assets/no-select.wav"},
      {"brick-hit-1", "../assets/brick-hit-1.wav"},
      {"brick-hit-2", "../assets/brick-hit-2.wav"},
      {"hurt", "../assets/hurt.wav"},
      {"victory", "../assets/victory.wav"},
      {"recover", "../assets/recover.wav"},
      {"high-score", "../assets/high_score.wav"},
      {"pause", "../assets/pause.wav"}
    };
    int len = sizeof sources / sizeof sources[0];
    InitAudioDevice();
    init(sources, len);
  }
  void init(const std::string sources[][2], int len) {
    for (int i = 0; i < len; i++) {
      sounds_[sources[i][0]] = LoadSound((sources[i][1]).c_str());
    }
    music_ = LoadMusicStream("../assets/music.wav");
    PlayMusicStream(music_);
  }
  void Play(std::string name) {
    Sound sound = sounds_[name];
    PlaySoundMulti(sound);
  }
  void UpdateMusic() {
    UpdateMusicStream(music_);
  }
  ~GlobalSound() noexcept {
    StopSoundMulti();       // We must stop the buffer pool before unloading
    StopMusicStream(music_);
    UnloadMusicStream(music_);
    for (auto& sound : sounds_) {
      UnloadSound(sound.second);
    }
  }
private:
  std::map<std::string, Sound> sounds_ = {};
  Music music_;
};
} // namespace breakout end