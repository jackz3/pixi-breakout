#pragma once
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include "raylib.h"
#include "sound.h"

namespace breakout {
const int VirtualWidth = 432;
const int VirtualHeight = 243;
const int ScreenWidth = 1280; //VirtualWidth * ScaleRatio;
const int ScreenHeight = 720; //VirtualHeight * ScaleRatio;
const float ScaleRatio = ScreenWidth / (float)VirtualWidth;
const int PADDLE_SPEED = 200;
const int LargeFontSize = 96;
const int MediumFontSize = 48;
const int SmallFontSize = 24;
Texture2D gb_texture;
Texture2D main_texture;
Texture2D hearts_texture;
Texture2D arrows_texture;
Texture2D particle_texture;
GlobalSound gSound;
std::map<std::string, std::unique_ptr<std::vector<Rectangle>>> GSprites;

template<typename T>
void log(const char* txt, T info) {
    std::cout << txt << "  " << info << std::endl;
}
template<typename T>
void log(T info) {
    std::cout << info << std::endl;
}

int GetCenterX(const char* text, int fontSize, int width = ScreenWidth) {
  return (ScreenWidth - MeasureText(text, fontSize)) / 2;
}

Rectangle ScaleRect(float x, float y, int width, int height) {
  return Rectangle { x * ScaleRatio, y * ScaleRatio, width * ScaleRatio, height * ScaleRatio };
}

void RenderScore(int score) {
    // love.graphics.print('Score:', VirtualWidth - 50, 5)
    // love.graphics.printf(tostring(score), VIRTUAL_WIDTH - 50, 5, 40, 'right')
  DrawText(TextFormat("score: %i", score), ScreenWidth - 180, 10, SmallFontSize, WHITE);
}

void RenderHealth(int health) {
    float healthX = VirtualWidth - 100;
    for(int i = 0; i < health; i++) {
        // love.graphics.draw(gTextures['hearts'], gFrames['hearts'][1], healthX, 4)
      DrawTexturePro(hearts_texture, Rectangle{0, 0, 100, 100}, Rectangle{0, 0, ScreenWidth, ScreenHeight}, {0.f, 0.f}, 0.f, WHITE);
      healthX = healthX + 11;
    }
    for(int i = 0; i < (3 - health); i++) {
      DrawTexturePro(hearts_texture, (*GSprites["hearts"])[1], ScaleRect(healthX, 4, 10, 9), {0.f, 0.f}, 0.f, WHITE);
      healthX = healthX + 11;
    }
}

auto GenerateQuadsPaddles() {
    int x = 0;
    int y = 64;
    std::unique_ptr<std::vector<Rectangle>> quads(new std::vector<Rectangle>);
    // int counter = 1;
    for(int i = 0; i < 4; i++) {
        // -- smallest
        quads->push_back(Rectangle {(float)x, (float)y, 32, 16});
        // counter = counter + 1
        // -- medium
        quads->push_back(Rectangle {(float)x + 32, (float)y, 64, 16});
        // counter = counter + 1
        // -- large
        // quads[counter] = love.graphics.newQuad(x + 96, y, 96, 16,
        quads->push_back(Rectangle {(float)x + 96, (float)y, 96, 16});
        // counter = counter + 1
        // -- huge
        // quads[counter] = love.graphics.newQuad(x, y + 16, 128, 16,
        quads->push_back(Rectangle {(float)x, (float)y + 16, 128, 16});
        // counter = counter + 1
        // -- prepare X and Y for the next set of paddles
        x = 0;
        y += 32;
    }
    return quads;
}
auto GenerateQuadsBalls() {
    int x = 96;
    int y = 48;
    std::unique_ptr<std::vector<Rectangle>> quads(new std::vector<Rectangle>);
    for(int i = 0; i < 4; i++) {
        quads->push_back(Rectangle {(float)x, (float)y, 8, 8});
        x = x + 8;
    }
    x = 96;
    y = 56;
    for(int i = 0; i < 3; i++) {
        quads->push_back(Rectangle{(float)x, (float)y, 8, 8});
        x = x + 8;
    }
    return quads;
}
auto GenerateQuads(Texture2D atlas, int tilewidth, int tileheight) {
    int sheetWidth = atlas.width / tilewidth;
    int sheetHeight = atlas.height / tileheight;

    std::unique_ptr<std::vector<Rectangle>> spritesheet(new std::vector<Rectangle>);
    for(int y = 0; y < sheetHeight; y++) {
        for(int x = 0; x < sheetWidth; x++) {
            spritesheet->push_back(Rectangle{float(x * tilewidth), float(y * tileheight), (float)tilewidth, (float)tileheight});
        }
    }
    return spritesheet;
}

void LoadRes() {
  breakout::gb_texture = LoadTexture("../assets/background.png");
  breakout::main_texture = LoadTexture("../assets/breakout.png");
  breakout::hearts_texture = LoadTexture("../assets/hearts.png");
  breakout::arrows_texture = LoadTexture("../assets/arrows.png");
  breakout::particle_texture = LoadTexture("../assets/particle.png");
  breakout::GSprites["paddles"] = GenerateQuadsPaddles();
  breakout::GSprites["balls"] = GenerateQuadsBalls();
  breakout::GSprites["bricks"] = GenerateQuads(breakout::main_texture, 32, 16);
  breakout::GSprites["hearts"] = GenerateQuads(breakout::hearts_texture, 10, 9);
  breakout::GSprites["arrows"] = GenerateQuads(breakout::arrows_texture, 24, 24);
}
void UnLoadRes() {
  UnloadTexture(breakout::gb_texture);
  UnloadTexture(breakout::main_texture);
  UnloadTexture(breakout::hearts_texture);
  UnloadTexture(arrows_texture);
  UnloadTexture(particle_texture);
}

struct HighScore {
  std::string name;
  int score;
};
auto LoadHighScores() {
  std::fstream file;
  std::string file_name = "breakout.lst";
  file.open(file_name, std::ios::in);
  if(!file.is_open()) {
    log("not open");
    file.open(file_name, std::ios::in|std::ios::out|std::ios::trunc);
    if (file.is_open()) {
      log("opened");
      for (int i = 0; i < 10; i++) {
        file << "NUL\n";
        file << std::to_string((10 - i) * 1000);
        file << "\n";
      }
      file.seekg(0);
    }
  }
  auto scores = std::shared_ptr<std::vector<HighScore>>(new std::vector<HighScore>);
  std::string name;
  std::string score;
  for (int i = 0; i < 10; i++) {
    getline(file, name);
    getline(file, score);
    log("name", name);
    log("score", score);
    scores->push_back(HighScore{name, std::stoi(score)});
  }
  file.close();
  return scores;
}
} // end of namespace