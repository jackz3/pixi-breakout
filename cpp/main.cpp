#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include "raylib.h"
#include "sound.h"

namespace breakout {
const int VirtualWidth = 432;
const int VirtualHeight = 243;
const float ScaleRatio = 2.0f;
const int ScreenWidth = VirtualWidth * ScaleRatio;
const int ScreenHeight = VirtualHeight * ScaleRatio;
const int PADDLE_SPEED = 200;
Texture2D main_texture;
std::map<std::string, std::vector<Rectangle>> GSprites = {};
std::map<std::string, Sound> GSounds = {};
GlobalSound gSound;

auto GenerateQuadsPaddles(std::map<std::string, std::vector<Rectangle>>& gSprites, Texture2D atlas) {
  int x = 0;
  int y = 64;
  int counter = 1;
  std::vector<Rectangle> quads = {};

  for (int i = 0; i < 4; i++) {
        // -- smallest
    quads.push_back(Rectangle{(float)x, (float)y, 32, 16});
        // quads[counter] = Rectangle{(float)x, (float)y, 32, 16};
        counter = counter + 1;
        // -- medium
        // quads[counter] = love.graphics.newQuad(x + 32, y, 64, 16,
        //     atlas:getDimensions())
        // counter = counter + 1;
        // -- large
        // quads[counter] = love.graphics.newQuad(x + 96, y, 96, 16,
        //     atlas:getDimensions())
        // counter = counter + 1;
        // -- huge
        // quads[counter] = love.graphics.newQuad(x, y + 16, 128, 16,
        //     atlas:getDimensions())
        // counter = counter + 1;

        // -- prepare X and Y for the next set of paddles
        x = 0;
        y = y + 32;
  }
  gSprites["paddles"] = quads;
  return true;
}
int GetCenterX(const char* text, int fontSize, int width = ScreenWidth) {
  return (ScreenWidth - MeasureText(text, fontSize)) / 2;
}

Rectangle ScaleRect(float x, float y, int width, int height) {
  return Rectangle { x * ScaleRatio, y * ScaleRatio, width * ScaleRatio, height * ScaleRatio };
}

class Paddle {
public:
  void update(float delta) {
    if (IsKeyDown(KEY_LEFT)) {
      dx = -PADDLE_SPEED;
    } else if (IsKeyDown(KEY_RIGHT)) {
      dx = PADDLE_SPEED;
    } else
      dx = 0;
    
    if (dx < 0) {
      x = std::max<float>(0, x + dx * delta);
    } else {
      x = std::min<float>(VirtualWidth - width, x + dx * delta);
    }
  }
  void render() {
    // love.graphics.draw(gTextures['main'], gFrames['paddles'][self.size + 4 * (self.skin - 1)],        self.x, self.y)
    // std::cout << x << " ,  " << y << std::endl;
    // Rectangle r = {0, 0, (float)breakout::ScreenWidth + breakout::ScaleRatio + 2, (float)breakout::ScreenHeight + breakout::ScaleRatio + 2};
    DrawTexturePro(main_texture, frameRec, ScaleRect(x, y, width, height), {0.f, 0.f}, 0.f, WHITE);
  }
private:
  float x = VirtualWidth / 2 - 32;
  float y = VirtualHeight - 32;
  float dx = 0;
  int width = 64;
  int height = 16;
  int size = 2;
  Rectangle frameRec = { 0.0f, 0.0f, (float)width, (float)height };
};

class BaseState {
public:
  void enter (std::map<std::string, std::string> params = {}) {}
  void exit() {}
  virtual void update (float delta) {}
  virtual void render() {}
  // void processAI(params:any, delta:number) {}
};

class StateMachine {
public:
  StateMachine() {
    empty = BaseState();
    current = &empty;
  }
  void SetStates(std::map<std::string, BaseState*>& initialStates) {
    states = initialStates;
  }
  void change(std::string stateName) {
    if(states[stateName] != nullptr) {
      current->exit();
      current = states[stateName];
      current->enter();
    }
  }
  void update (float delta) {
    current->update(delta);
  }
  void render () {
    current->render();
  }
  // processAI (params:any, delta:number) {
  //   this.current.processAI(params, delta)
  // }
private:
  std::map<std::string, BaseState*> states;
  BaseState empty;
  BaseState* current;
};

StateMachine stateMachine;

class StartState : public BaseState {
public:
  void update(float delta) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
      highlighted = highlighted == 1 ? 2 : 1;
      gSound.play("paddle-hit");
		}
    if (IsKeyPressed(KEY_ENTER)) {
      gSound.play("confirm");

      if (highlighted == 1) {
        stateMachine.change("play");
      } else {
        // global.stateMachine.change('high-scores', {highScores: this.highScores})
      }
    }
  }
  void render() override {
    DrawText(title_, GetCenterX(title_, titleFontSize), (ScreenHeight - 16) / 2 - 60, titleFontSize, WHITE);
    DrawText(start_, GetCenterX(start_, menuFontSize), (ScreenHeight / 2) + 50, menuFontSize, highlighted == 1 ? highlightColor : WHITE);
    DrawText(highScore_, GetCenterX(highScore_, menuFontSize), (ScreenHeight / 2) + 90, menuFontSize, highlighted == 2 ? highlightColor : WHITE);
  }
private:
  const char* title_ = "BREAKOUT";
  const char* start_ = "START";
  const char* highScore_ = "HIGH SCORES";
  int titleFontSize = 80;
  int menuFontSize = 32;
  int textWidth = MeasureText(title_, titleFontSize);
  int highlighted = 1;
  Color highlightColor = (Color){ 103, 255, 255, 255 };
};

class PlayState : public BaseState {
public:
  void update(float delta) {
    paddle_.update(delta);
  }
  void render() {
    paddle_.render();

  }
private:
  Paddle paddle_;
};


} //end namespace breadout
int main() {
  std::cout << breakout::ScreenWidth << " , " << breakout::ScreenHeight << std::endl;
  InitWindow(breakout::ScreenWidth, breakout::ScreenHeight, "Breakout");
	SetWindowState(FLAG_VSYNC_HINT);

  breakout::main_texture = LoadTexture("../assets/breakout.png");
  Texture2D gb_texture = LoadTexture("../assets/background.png");
  auto startState = breakout::StartState();
  auto playState = breakout::PlayState();
  std::map<std::string, breakout::BaseState*> initialState = { {"start", &startState}, {"play", &playState} };
  breakout::stateMachine.SetStates(initialState);
  breakout::stateMachine.change("start");
  while (!WindowShouldClose()) {
    float delta = GetFrameTime();
    breakout::stateMachine.update(delta);
    BeginDrawing();
			ClearBackground(BLACK);


      DrawTexturePro(gb_texture, {0, 0, (float)gb_texture.width, (float)gb_texture.height}, {0, 0, (float)breakout::ScreenWidth + breakout::ScaleRatio + 2, (float)breakout::ScreenHeight + breakout::ScaleRatio + 2}, {0, 0}, 0, WHITE);
      breakout::stateMachine.render();

			DrawFPS(10, 10);
		EndDrawing();
  }
  UnloadTexture(gb_texture);
  UnloadTexture(breakout::main_texture);
  breakout::gSound.unload();
  CloseWindow();
  return 0;
}