#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <memory>
#include "raylib.h"
#include "common.h"
#include "sound.h"
#include "util.h"
#include "objects.h"
#if defined(PLATFORM_WEB)
  #include <emscripten/emscripten.h>
#endif

namespace breakout {


class BaseState {
public:
  virtual void enter (std::shared_ptr<void> ptr) {}
  virtual void exit() {}
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
  void change(std::string stateName, std::shared_ptr<void> ptr = nullptr) {
    if(states[stateName] != nullptr) {
      current->exit();
      current = states[stateName];
      current->enter(ptr);
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

struct ServeParams
{
  Paddle paddle;
  std::vector<Brick> bricks; 
  int health;
  int score;
};

struct PlayParams {
  Paddle paddle;
  std::vector<Brick> bricks; 
  int health;
  int score;
  Ball ball;
};

struct GameOverParams {
  int score;
};
class GameOverState : public BaseState {
public:
  void enter(std::shared_ptr<void> ptr) override {
    GameOverParams params = *(GameOverParams*)(ptr.get());
    score_ = params.score;
  }
  void update(float delta) {
    if(IsKeyPressed(KEY_ENTER)) {
      stateMachine.change("start");
    }
  }
  void render() {
    // love.graphics.printf('GAME OVER', 0, VIRTUAL_HEIGHT / 3, VIRTUAL_WIDTH, 'center')
    DrawText(str_gameover_, GetCenterX(str_gameover_, LargeFontSize), ScreenHeight / 3, LargeFontSize, WHITE);
    const char* str_score = std::string(str_score_ + std::to_string(score_)).c_str();
    DrawText(str_score, GetCenterX(str_score, MediumFontSize), ScreenHeight / 2, MediumFontSize, WHITE);
    // love.graphics.printf('Final Score: ' .. tostring(self.score), 0, VIRTUAL_HEIGHT / 2,    VIRTUAL_WIDTH, 'center')
    // love.graphics.printf('Press Enter!', 0, VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 4,        VIRTUAL_WIDTH, 'center')
    DrawText(str_press_, GetCenterX(str_press_, MediumFontSize), ScreenHeight / 4, MediumFontSize, WHITE);
  }
private:
  int score_;
  const char* str_gameover_ = "GAME OVER";
  const char* str_score_ = "Final Score: ";
  const char* str_press_ = "Press Enter!";
};

class ServeState : public BaseState {
public:
  void enter (std::shared_ptr<void> ptr) override {
    ServeParams params = *(ServeParams*)(ptr.get());
    paddle_ = params.paddle;
    bricks_ = params.bricks;
    health_ = params.health;
    score_ = params.score;
  }
  void update(float delta) {
    paddle_.update(delta);
    ball_.x = paddle_.x + (paddle_.width / 2) - 4;
    ball_.y = paddle_.y - 8;

    if(IsKeyPressed(KEY_ENTER)) {
        // -- pass in all important state info to the PlayState
      stateMachine.change("play", std::shared_ptr<void>(new PlayParams{
          paddle_,
          bricks_,
          health_,
          score_,
          ball_,
      }));
    }
  }
  void render() {
    paddle_.render();
    ball_.render();
    for (auto& brick : bricks_) {
      brick.render();
    }

    RenderScore(score_);
    RenderHealth(health_);
    // love.graphics.setFont(gFonts['medium'])
    // love.graphics.printf('Press Enter to serve!', 0, VIRTUAL_HEIGHT / 2,    VIRTUAL_WIDTH, 'center')
    DrawText(txt, GetCenterX(txt, font_size_), (ScreenHeight - font_size_) / 2, font_size_, WHITE);
  }
private:
  Paddle paddle_;
  std::vector<Brick> bricks_; 
  int health_;
  int score_;
  Ball ball_{GetRandomValue(0, 7)};
  const char* txt = "Press Enter to serve!";
  int font_size_ = 48;
};

class StartState : public BaseState {
public:
  void update(float delta) override {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
      highlighted = highlighted == 1 ? 2 : 1;
      gSound.play("paddle-hit");
		}
    if (IsKeyPressed(KEY_ENTER)) {
      gSound.play("confirm");

      if (highlighted == 1) {
        stateMachine.change("serve", std::shared_ptr<void>(new ServeParams{Paddle(0), createMap(), 3, 0}));
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
  void enter(std::shared_ptr<void> ptr) override {
    auto params = *(PlayParams*)ptr.get();
    paddle_ = params.paddle;
    bricks_ = params.bricks;
    health_ = params.health;
    score_ = params.score;
    ball_ = params.ball;

    // -- give ball random starting velocity
    ball_.dx = GetRandomValue(-200, 200);
    ball_.dy = GetRandomValue(-50, -60);
  }
  void update(float delta) {
    if(paused_) {
      if(IsKeyPressed(KEY_SPACE)) {
        paused_ = false;
        gSound.play("pause");
      } else
        return;
    } else if(IsKeyPressed(KEY_SPACE)) {
        paused_ = true;
        gSound.play("pause");
        return;
    }
    paddle_.update(delta);
    ball_.update(delta);

    if (ball_.collides(Rectangle{paddle_.x, paddle_.y, (float)paddle_.width, (float)paddle_.height})) {
      // -- raise ball above paddle in case it goes below it, then reverse dy
      ball_.y -= 8;
      ball_.dy = -ball_.dy;
        // -- tweak angle of bounce based on where it hits the paddle
        // -- if we hit the paddle on its left side while moving left...
      if(ball_.x < paddle_.x + (paddle_.width / 2) && paddle_.dx < 0) {
        ball_.dx = -50 + -(8 * (paddle_.x + paddle_.width / 2 - ball_.x));
      }
      // -- else if we hit the paddle on its right side while moving right...
      else if(ball_.x > paddle_.x + (paddle_.width / 2)  && paddle_.dx > 0) {
        ball_.dx = 50 + (8 * (ball_.x - (paddle_.x + paddle_.width / 2)));
      }

      gSound.play("paddle-hit");
    }

    for(auto& b : bricks_) {
      if(b.inPlay && ball_.collides(Rectangle{b.x, b.y, (float)b.width, (float)b.height})) {
        score_ += (b.tier * 200 + b.color * 25);
        b.hit();

        if(ball_.x + 2 < b.x and ball_.dx > 0) {
          ball_.dx = -ball_.dx;
          ball_.x = b.x - 8;
        } 
        // -- right edge; only check if we're moving left, , and offset the check by a couple of pixels
        // -- so that flush corner hits register as Y flips, not X flips
        else if(ball_.x + 6 > b.x + b.width && ball_.dx < 0) {
          // -- flip x velocity and reset position outside of brick
          ball_.dx = -ball_.dx;
          ball_.x = b.x + 32;
        }
            // -- top edge if no X collisions, always check
        else if(ball_.y < b.y) {
          // -- flip y velocity and reset position outside of brick
          ball_.dy = -ball_.dy;
          ball_.y = b.y - 8;
        }
        // -- bottom edge if no X collisions or top collision, last possibility
        else {
          // -- flip y velocity and reset position outside of brick
          ball_.dy = -ball_.dy;
          ball_.y = b.y + 16;
        }
        // -- slightly scale the y velocity to speed up the game, capping at +- 150
        if(abs(ball_.dy) < 150) {
          ball_.dy = ball_.dy * 1.02;
        }
        break;
      }
    }

    if(ball_.y >= VirtualHeight) {
      health_--;
      log("health", health_);
      gSound.play("hurt");
      if(health_ == 0) {
        stateMachine.change("game-over", std::shared_ptr<void>(new GameOverParams{score_}));
      } else {
        stateMachine.change("serve", std::shared_ptr<void>(new ServeParams{paddle_, bricks_, health_, score_}));
      }
    }

  }
  void render() {
    for(auto& b : bricks_) {
      b.render();
    }
    paddle_.render();
    ball_.render();

    RenderScore(score_);
    RenderHealth(health_);
  }
private:
  Paddle paddle_;
  Ball ball_{1};
  std::vector<Brick> bricks_;
  int health_;
  int score_;
  bool paused_ = false;
};


} //end namespace breadout

void LoadRes() {
  breakout::gb_texture = LoadTexture("../assets/background.png");
  breakout::main_texture = LoadTexture("../assets/breakout.png");
  breakout::hearts_texture = LoadTexture("../assets/hearts.png");
  breakout::GSprites["paddles"] = GenerateQuadsPaddles();
  breakout::GSprites["balls"] = GenerateQuadsBalls();
  breakout::GSprites["bricks"] = GenerateQuads(breakout::main_texture, 32, 16);
  breakout::GSprites["hearts"] = GenerateQuads(breakout::hearts_texture, 10, 9);
}
void UpdateDrawFrame(void)
{
  float delta = GetFrameTime();
  breakout::stateMachine.update(delta);
  BeginDrawing();
    ClearBackground(WHITE);

    DrawTexturePro(breakout::gb_texture, {0, 0, (float)breakout::gb_texture.width, (float)breakout::gb_texture.height}, {0, 0, (float)breakout::ScreenWidth + breakout::ScaleRatio + 2, (float)breakout::ScreenHeight + breakout::ScaleRatio + 2}, {0, 0}, 0, WHITE);
    breakout::stateMachine.render();
    DrawFPS(10, 10);
  EndDrawing();
}
int main() {
  // std::cout << breakout::ScreenWidth << " , " << breakout::ScreenHeight << std::endl;
  InitWindow(breakout::ScreenWidth, breakout::ScreenHeight, "Breakout");
	SetWindowState(FLAG_VSYNC_HINT);
  LoadRes();

  auto startState = breakout::StartState();
  auto playState = breakout::PlayState();
  auto serveState = breakout::ServeState();
  auto gameoverState = breakout::GameOverState();
  std::map<std::string, breakout::BaseState*> initialState = {
    {"start", &startState},
    {"play", &playState},
    {"serve", &serveState},
    {"game-over", &gameoverState} };
  breakout::stateMachine.SetStates(initialState);
  breakout::stateMachine.change("start");
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
  #else
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
  #endif
  UnloadTexture(breakout::gb_texture);
  UnloadTexture(breakout::main_texture);
  UnloadTexture(breakout::hearts_texture);
  return 0;
}