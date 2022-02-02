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
  std::vector<HighScore> highScore;
  int level;
  int recoverPoints;
};
struct PlayParams {
  Paddle paddle;
  std::vector<Brick> bricks; 
  int health;
  int score;
  Ball ball;
  std::vector<HighScore> highScore;
  int level;
  int recoverPoints;
};
struct VictoryParams {
  Paddle paddle;
  int health;
  int score;
  Ball ball;
  std::vector<HighScore> highScore;
  int level;
  int recoverPoints;
};

class VictoryState : public BaseState {
public:
  void enter(std::shared_ptr<void> ptr) {
    auto params = *(VictoryParams*)(ptr.get());
    paddle_ = params.paddle;
    health_ = params.health;
    score_ = params.score;
    highScore_ = params.highScore;
    level_ = params.level;
    recoverPoints_ = params.recoverPoints;
    ball_ = params.ball;
  }
  void update(float delta) {
    paddle_.update(delta);
    // -- have the ball track the player
    ball_.x = paddle_.x + (paddle_.width / 2) - 4;
    ball_.y = paddle_.y - 8;
    if(IsKeyPressed(KEY_ENTER)) {
      stateMachine.change("serve", std::shared_ptr<void>(new ServeParams{paddle_, createMap(level_ + 1), health_, score_, highScore_, level_ + 1, recoverPoints_}));
    }
  }
  void render() {
    paddle_.render();
    ball_.render();

    RenderHealth(health_);
    RenderScore(score_);
    // love.graphics.printf("Level " .. tostring(self.level) .. " complete!",    0, VIRTUAL_HEIGHT / 4, VIRTUAL_WIDTH, 'center')
    const char* str_levele = TextFormat("Level %i complete!", level_);
    DrawText(str_levele, GetCenterX(str_levele, LargeFontSize), ScreenHeight / 4, LargeFontSize, WHITE);
    // love.graphics.printf('Press Enter to serve!', 0, VIRTUAL_HEIGHT / 2,        VIRTUAL_WIDTH, 'center')
    const char* str_press = "Press Enter to serve!";
    DrawText(str_press, GetCenterX(str_press, MediumFontSize), ScreenHeight / 2, MediumFontSize, WHITE);
  }
private:
  Paddle paddle_;
  Ball ball_;
  int health_;
  int score_;
  const char* txt = "Press Enter to serve!";
  std::vector<HighScore> highScore_;
  int level_;
  int recoverPoints_;

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
    highScore_ = params.highScore;
    level_ = params.level;
    recoverPoints_ = params.recoverPoints;
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
          highScore_,
          level_,
          recoverPoints_
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
  std::vector<HighScore> highScore_;
  int level_;
  int recoverPoints_;
};

struct StartParams {
  std::vector<HighScore> highScore;
};
class HighScoreState : public BaseState {
public:
  void enter(std::shared_ptr<void> ptr) override {
    StartParams params = *(StartParams*)(ptr.get());
    highScore_ = params.highScore;
  }
  void update(float delta) override {
    if(IsKeyPressed(KEY_ESCAPE)) {
      gSound.Play("wall-hit");
      stateMachine.change("start", std::shared_ptr<void>(new StartParams{highScore_}));
    }
  } 
  void render() override {
    DrawText(str_highscore_, GetCenterX(str_highscore_, LargeFontSize), 20 * ScaleRatio, LargeFontSize, WHITE);
    int idx = 0;
    for(auto& hs : highScore_) {
      DrawText(TextFormat("%02i.", (idx + 1)), (VirtualWidth / 4) * ScaleRatio,  (60 + idx * 16) * ScaleRatio, MediumFontSize, WHITE);
      DrawText(hs.name.c_str(), (VirtualWidth / 4 + 38) * ScaleRatio,  (60 + idx * 16) * ScaleRatio, MediumFontSize, WHITE);
      DrawText(std::to_string(hs.score).c_str(), (VirtualWidth / 2) * ScaleRatio,  (60 + idx * 16) * ScaleRatio, MediumFontSize, WHITE);
      idx++;
    }
  }
private:
  std::vector<HighScore> highScore_;
  const char* str_highscore_ = "High Scores";

};
class StartState : public BaseState {
public:
  void enter(std::shared_ptr<void> ptr) override {
    StartParams params = *(StartParams*)(ptr.get());
    highScore_ = params.highScore;
  }
  void update(float delta) override {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
      highlighted = highlighted == 1 ? 2 : 1;
      gSound.Play("paddle-hit");
		}
    if (IsKeyPressed(KEY_ENTER)) {
      gSound.Play("confirm");

      if (highlighted == 1) {
        stateMachine.change("paddle-select", std::shared_ptr<void>(new StartParams{highScore_}));
      } else {
        stateMachine.change("high-scores", std::shared_ptr<void>(new StartParams{highScore_}));
      }
    }
  }
  void render() override {
    DrawText(title_, GetCenterX(title_, LargeFontSize), ScreenHeight / 3 , LargeFontSize, WHITE);
    DrawText(start_, GetCenterX(start_, MediumFontSize), ScreenHeight / 2 + 140, MediumFontSize, highlighted == 1 ? highlightColor : WHITE);
    DrawText(str_highScore_, GetCenterX(str_highScore_, MediumFontSize), ScreenHeight / 2 + 180, MediumFontSize, highlighted == 2 ? highlightColor : WHITE);
  }
private:
  const char* title_ = "BREAKOUT";
  const char* start_ = "START";
  const char* str_highScore_ = "HIGH SCORES";
  int textWidth = MeasureText(title_, LargeFontSize);
  int highlighted = 1;
  Color highlightColor = (Color){ 103, 255, 255, 255 };
  std::vector<HighScore> highScore_;
};

class PaddleSelectState : public BaseState {
public:
  void enter(std::shared_ptr<void> ptr) override {
    StartParams params = *(StartParams*)(ptr.get());
    highScore_ = params.highScore;
  }
  void update(float delta) {
    if(IsKeyPressed(KEY_LEFT)) {
        if(currentPaddle == 0) {
          gSound.Play("no-select");
        } else {
          gSound.Play("select");
          currentPaddle--;
        }
    } else if (IsKeyPressed(KEY_RIGHT)) {
        if(currentPaddle == 3) {
          gSound.Play("no-select");
        } else {
          gSound.Play("select");
          currentPaddle++;
        }
    }
    // -- select paddle and move on to the serve state, passing in the selection
    if(IsKeyPressed(KEY_ENTER)) {
        gSound.Play("confirm");
        stateMachine.change("serve", std::shared_ptr<void>(new ServeParams{Paddle(currentPaddle), createMap(32), 3, 0, highScore_, 32, 5000}));
    }
  }
  void render() {
    // love.graphics.printf("Select your paddle with left and right!", 0, VIRTUAL_HEIGHT / 4,    VIRTUAL_WIDTH, 'center')
    const char* str_select = "Select your paddle with left and right!";
    DrawText(str_select, GetCenterX(str_select, MediumFontSize), ScreenHeight / 4, MediumFontSize, WHITE);
    // love.graphics.printf("(Press Enter to continue!)", 0, VIRTUAL_HEIGHT / 3,        VIRTUAL_WIDTH, 'center')
    const char* str_press = "(Press Enter to continue!)";
    DrawText(str_press, GetCenterX(str_press, SmallFontSize), ScreenHeight / 3, SmallFontSize, WHITE); 
    // -- left arrow; should render normally if we're higher than 1, else
    // -- in a shadowy form to let us know we're as far left as we can go
    // -- tint; give it a dark gray with half opacity
    // love.graphics.setColor(40/255, 40/255, 40/255, 128/255)
    // love.graphics.draw(gTextures['arrows'], gFrames['arrows'][1], VIRTUAL_WIDTH / 4 - 24,        VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)
    DrawTexturePro(arrows_texture, (*GSprites["arrows"])[0], ScaleRect(VirtualWidth / 4 - 24, VirtualHeight * 2 / 3, 24, 24), {0, 0}, 0.f, currentPaddle == 0 ? Color{40, 40, 40, 128} : WHITE); 
   
    // -- right arrow; should render normally if we're less than 4, else
    // -- in a shadowy form to let us know we're as far right as we can go
    // if self.currentPaddle == 4 then
        // love.graphics.setColor(40/255, 40/255, 40/255, 128/255)
    // love.graphics.draw(gTextures['arrows'], gFrames['arrows'][2], VIRTUAL_WIDTH - VIRTUAL_WIDTH / 4,    VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)
    DrawTexturePro(arrows_texture, (*GSprites["arrows"])[1], ScaleRect(VirtualWidth * 3 / 4, VirtualHeight * 2 / 3, 24, 24), {0, 0}, 0.f, currentPaddle == 3 ? Color{40, 40, 40, 128} : WHITE); 
    
    // -- draw the paddle itself, based on which we have selected
    // love.graphics.draw(gTextures['main'], gFrames['paddles'][2 + 4 * (self.currentPaddle - 1)],        VIRTUAL_WIDTH / 2 - 32, VIRTUAL_HEIGHT - VIRTUAL_HEIGHT / 3)
    DrawTexturePro(main_texture, (*GSprites["paddles"])[2 + 4 * currentPaddle], ScaleRect(VirtualWidth / 2 -32, VirtualHeight * 2 / 3, 64, 16), {0, 0}, 0.f, WHITE);
  }
private:
  int currentPaddle = 0;
  std::vector<HighScore> highScore_;
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
    highScore_ = params.highScore;
    level_ = params.level;
    recoverPoints_ = params.recoverPoints;
    // -- give ball random starting velocity
    ball_.dx = GetRandomValue(-200, 200);
    ball_.dy = GetRandomValue(-50, -60);
  }
  void update(float delta) {
    if(paused_) {
      if(IsKeyPressed(KEY_SPACE)) {
        paused_ = false;
        gSound.Play("pause");
      } else
        return;
    } else if(IsKeyPressed(KEY_SPACE)) {
        paused_ = true;
        gSound.Play("pause");
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

      gSound.Play("paddle-hit");
    }

    for(auto& b : bricks_) {
      if(b.inPlay && ball_.collides(Rectangle{b.x, b.y, (float)b.width, (float)b.height})) {
        score_ += (b.tier * 200 + b.color * 25);
        b.hit();
        // -- if we have enough points, recover a point of health
        if(score_ > recoverPoints_) {
          gSound.Play("recover");
          // -- can't go above 3 health
          health_ = std::min(3, health_ + 1);
          recoverPoints_ = std::min(100000, recoverPoints_ * 2);
        } 
        if(checkVictory()) {
          gSound.Play("victory");
          stateMachine.change("victory", std::shared_ptr<void>(new VictoryParams{ paddle_, health_, score_, ball_, highScore_, level_, recoverPoints_ }));
        }

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
      gSound.Play("hurt");
      if(health_ == 0) {
        stateMachine.change("game-over", std::shared_ptr<void>(new GameOverParams{score_}));
      } else {
        stateMachine.change("serve", std::shared_ptr<void>(new ServeParams{paddle_, bricks_, health_, score_, highScore_, level_ + 1, recoverPoints_}));
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
  int level_;
  int recoverPoints_;
  std::vector<breakout::HighScore> highScore_;
  bool checkVictory() {
    for(auto& brick : bricks_) {
      if(brick.inPlay) {
        return false;
      }
    }
    return true;
  }
};


} //end namespace breadout

void UpdateDrawFrame(void)
{
  float delta = GetFrameTime();
  breakout::stateMachine.update(delta);
  breakout::gSound.UpdateMusic();
  BeginDrawing();
    ClearBackground(WHITE);

    DrawTexturePro(breakout::gb_texture, {0, 0, (float)breakout::gb_texture.width, (float)breakout::gb_texture.height}, {0, 0, (float)breakout::ScreenWidth + breakout::ScaleRatio + 1, (float)breakout::ScreenHeight + breakout::ScaleRatio + 3}, {0, 0}, 0, WHITE);
    breakout::stateMachine.render();
    DrawFPS(10, 10);
  EndDrawing();
}
int main() {
  InitWindow(breakout::ScreenWidth, breakout::ScreenHeight, "Breakout");
  SetExitKey(KEY_NULL);
  breakout::LoadRes();

  auto startState = breakout::StartState();
  auto playState = breakout::PlayState();
  auto serveState = breakout::ServeState();
  auto gameoverState = breakout::GameOverState();
  auto highScores = breakout::HighScoreState();
  auto paddleSelectState = breakout::PaddleSelectState();
  std::map<std::string, breakout::BaseState*> initialState = {
    {"start", &startState},
    {"play", &playState},
    {"serve", &serveState},
    {"game-over", &gameoverState},
    {"paddle-select", &paddleSelectState},
    {"high-scores", &highScores} };
  breakout::stateMachine.SetStates(initialState);
  breakout::stateMachine.change("start", breakout::LoadHighScores());
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
  #else
	// SetWindowState(FLAG_VSYNC_HINT);
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
  #endif
  breakout::UnLoadRes();
  return 0;
}