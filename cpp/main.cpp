#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include "raylib.h"
#include "sound.h"
#include "util.h"
#if defined(PLATFORM_WEB)
  #include <emscripten/emscripten.h>
#endif

namespace breakout {
const int VirtualWidth = 432;
const int VirtualHeight = 243;
const float ScaleRatio = 2.0f;
const int ScreenWidth = VirtualWidth * ScaleRatio;
const int ScreenHeight = VirtualHeight * ScaleRatio;
const int PADDLE_SPEED = 200;
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
    // love.graphics.setFont(gFonts['small'])
    // love.graphics.print('Score:', VirtualWidth - 50, 5)
    // love.graphics.printf(tostring(score), VIRTUAL_WIDTH - 50, 5, 40, 'right')
  DrawText(("Score: " + std::to_string(score)).c_str(), ScreenWidth - 120, 6, 16, WHITE);
}

void RenderHealth(int health) {
    float healthX = ScreenWidth - 200;
    for(int i = 0; i < health; i++) {
      std::cout << healthX << std::endl;
        // love.graphics.draw(gTextures['hearts'], gFrames['hearts'][1], healthX, 4)
      DrawTextureRec(hearts_texture, (*GSprites["hearts"])[0], { healthX, 8 }, WHITE);
      healthX = healthX + 22;
    }
    for(int i = 0; i < (3 - health); i++) {
        // love.graphics.draw(gTextures['hearts'], gFrames['hearts'][2], healthX, 4)
      healthX = healthX + 22;
    }
}


class Brick {
public:
  float x;
  float y;
  int width = 32;
  int height = 16;
  bool inPlay = true;
  Brick(float x, float y) : x{x}, y{y} {}
  void hit() {
    gSound.play("brick-hit-2");
    inPlay = false;
  }
  void update(float delta) {}
  void render() {
    if(inPlay) {
          // -- multiply color by 4 (-1) to get our color offset, then add tier to that
          // -- to draw the correct tier and color brick onto the screen
      DrawTexturePro(main_texture, (*GSprites["bricks"])[((color - 1) * 4) + tier], ScaleRect(x, y, width, height), {0.f, 0.f}, 0.f, WHITE);
    }
  }
private:
  int tier = 0;
  int color = 1;
};
class Paddle {
public:
  float x = VirtualWidth / 2 - 32;
  float y = VirtualHeight - 32;
  int width = 64;
  int height = 16;
  float dx = 0;
  Paddle(int skin = 0) : skin{skin} {}
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
    DrawTexturePro(main_texture, frame, ScaleRect(x, y, width, height), {0.f, 0.f}, 0.f, WHITE);
  }
private:
  int skin = 0;
  int size = 2;
  Rectangle frame = (*GSprites["paddles"])[size + 4 * skin];
};

class Ball {
public:
  float x = 0;
  float y = 0;
  int dy = 0;
  int dx = 0;
  Ball(int skin) : skin{skin} {}
  bool collides(Rectangle rect) {
    return CheckCollisionRecs(Rectangle{x, y, (float)width, (float)height}, rect);
    // -- first, check to see if the left edge of either is farther to the right
    // -- than the right edge of the other
    // if self.x > target.x + target.width or target.x > self.x + self.width then
    //     return false
    // end
    // -- then check to see if the bottom edge of either is higher than the top
    // -- edge of the other
    // if self.y > target.y + target.height or target.y > self.y + self.height then
    //     return false
    // end 
    // -- if the above aren't true, they're overlapping
    // return true
  }
    void update(float delta) {
      x += dx * delta;
      y += dy * delta;

    if (x <= 0) {
      x = 0;
      dx = -dx;
      gSound.play("wall-hit");
    }

    if (x >= VirtualWidth - 8) {
        x = VirtualWidth - 8;
        dx = -dx;
        gSound.play("wall-hit");
    }

    if (y <= 0) {
      y = 0;
      dy = -dy;
      gSound.play("wall-hit");
    }

    }
    void render() {
    // love.graphics.draw(gTextures['main'], gFrames['balls'][self.skin],        self.x, self.y)
      Rectangle frame = (*GSprites["balls"])[skin];
      DrawTexturePro(main_texture, frame, ScaleRect(x, y, width, height), {0.f, 0.f}, 0.f, WHITE);
    }
    void Reset() {
      x = VirtualWidth / 2 - 2;
      y = VirtualHeight / 2 - 2;
      dx = 0;
      dy = 0;
    }
private:
  int width = 8;
  int height = 8;
  int skin;
};

auto createMap(int level = 1) {
    std::vector<Brick> bricks;
    // -- randomly choose the number of rows
    int numRows = GetRandomValue(1, 5);
    // -- randomly choose the number of columns
    int numCols = GetRandomValue(7, 13);
    // -- lay out bricks such that they touch each other and fill the space
    for(int y = 1; y <= numRows; y++) {
        for(int x = 0; x < numCols; x++) {
            auto b = Brick(x * 32 +8 + (13 - numCols) * 16, y * 16);
                // -- x-coordinate
                // (x-1)                   -- decrement x by 1 because tables are 1-indexed, coords are 0
                // * 32                    -- multiply by 32, the brick width
                // + 8                     -- the screen should have 8 pixels of padding; we can fit 13 cols + 16 pixels total
                // + (13 - numCols) * 16,  -- left-side padding for when there are fewer than 13 columns
                // -- y-coordinate
                // y * 16                  -- just use y * 16, since we need top padding anyway
          bricks.push_back(b);
        }
    } 
    return bricks;
}
template<typename T>
class BaseState {
public:
  // void enter (std::string) {}
  virtual void enter (T params) {}
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
  template<typename T>
  void change(std::string stateName, T params) {
    if(states[stateName] != nullptr) {
      current->exit();
      current = states[stateName];
      current->enter(params);
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

class ServeState : public BaseState<ServeParams> {
public:
  void enter (ServeParams params) {
    std::cout << params.health << " , " << params.score << std::endl;
    paddle_ = params.paddle;
    bricks_ = params.bricks;
    health_ = params.health;
    score_ = params.score;
    // ball_ = Ball(GetRandomValue(0, 7));
  }
  void update(float delta) {
    paddle_.update(delta);
    ball_.x = paddle_.x + (paddle_.width / 2) - 4;
    ball_.y = paddle_.y - 8;

    if(IsKeyPressed(KEY_ENTER)) {
        // -- pass in all important state info to the PlayState
      stateMachine.change("play", PlayParams{
          paddle_,
          bricks_,
          health_,
          score_,
          ball_,
      });
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

class StartState : public BaseState<int> {
public:
  void update(float delta) override {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
      highlighted = highlighted == 1 ? 2 : 1;
      gSound.play("paddle-hit");
		}
    if (IsKeyPressed(KEY_ENTER)) {
      gSound.play("confirm");

      if (highlighted == 1) {
        // stateMachine.change("play", 0);
        stateMachine.change("serve", ServeParams{Paddle(0), createMap(), 3, 0});
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

class PlayState : public BaseState<PlayParams> {
public:
  void enter(PlayParams params) {
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
  }
  void render() {
    for(auto& b : bricks_) {
      b.render();
    }
    paddle_.render();
    ball_.render();
  }
private:
  Paddle paddle_;
  Ball ball_{1};
  std::vector<Brick> bricks_;
  int health_;
  int score_;
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
  // std::cout << "delta:" << delta << std::endl;
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
  std::map<std::string, breakout::BaseState*> initialState = {
    {"start", &startState},
    {"play", &playState},
    {"serve", &serveState} };
  breakout::stateMachine.SetStates(initialState);
  breakout::stateMachine.change("start", 0);
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
  #else
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
  #endif
  UnloadTexture(breakout::gb_texture);
  UnloadTexture(breakout::main_texture);
  breakout::gSound.unload();
  CloseWindow();
  return 0;
}