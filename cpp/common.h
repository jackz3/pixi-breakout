#pragma once
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include "raylib.h"
#include "sound.h"

namespace breakout {
const int VirtualWidth = 432;
const int VirtualHeight = 243;
const int ScreenWidth = 1280; //VirtualWidth * ScaleRatio;
const int ScreenHeight = 720; //VirtualHeight * ScaleRatio;
const float ScaleRatio = ScreenWidth / (float)VirtualWidth;
const int PADDLE_SPEED = 200;
const int LargeFontSize = 90;
const int MediumFontSize = 36;
Texture2D gb_texture;
Texture2D main_texture;
Texture2D hearts_texture;
std::map<std::string, Sound> GSounds = {};
GlobalSound gSound;
std::map<std::string, std::unique_ptr<std::vector<Rectangle>>> GSprites;

int GetCenterX(const char* text, int fontSize, int width = ScreenWidth) {
  return (ScreenWidth - MeasureText(text, fontSize)) / 2;
}

Rectangle ScaleRect(float x, float y, int width, int height) {
  return Rectangle { x * ScaleRatio, y * ScaleRatio, width * ScaleRatio, height * ScaleRatio };
}

void RenderScore(int score) {
    // love.graphics.print('Score:', VirtualWidth - 50, 5)
    // love.graphics.printf(tostring(score), VIRTUAL_WIDTH - 50, 5, 40, 'right')
  DrawText(("Score: " + std::to_string(score)).c_str(), ScreenWidth - 180, 10, 24, WHITE);
}

void RenderHealth(int health) {
    float healthX = VirtualWidth - 140;
    for(int i = 0; i < health; i++) {
        // love.graphics.draw(gTextures['hearts'], gFrames['hearts'][1], healthX, 4)
      DrawTexturePro(hearts_texture, (*GSprites["hearts"])[0], ScaleRect(healthX, 4, 18, 16), {0.f, 0.f}, 0.f, WHITE);
      healthX = healthX + 22;
    }
    for(int i = 0; i < (3 - health); i++) {
      DrawTexturePro(hearts_texture, (*GSprites["hearts"])[1], ScaleRect(healthX, 4, 18, 16), {0.f, 0.f}, 0.f, WHITE);
      healthX = healthX + 22;
    }
}
} // end of namespace