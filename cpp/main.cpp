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
std::map<std::string, Sound> GSounds = {};
GlobalSound gSound;
std::map<std::string, std::unique_ptr<std::vector<Rectangle>>> GSprites;

int GetCenterX(const char* text, int fontSize, int width = ScreenWidth) {
  return (ScreenWidth - MeasureText(text, fontSize)) / 2;
}

Rectangle ScaleRect(float x, float y, int width, int height) {
  return Rectangle { x * ScaleRatio, y * ScaleRatio, width * ScaleRatio, height * ScaleRatio };
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
  // Paddle(int skin) : skin{skin} {}
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
  float dx = 0;
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
  void update(float delta) override {
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
  PlayState() {
    ball_.dx = GetRandomValue(-200, 200);
    ball_.dy = GetRandomValue(-50, -60);
    // -- give ball position in the center
    ball_.x = VirtualWidth / 2 - 4;
    ball_.y = VirtualHeight - 42;

    bricks_ = createMap();
  }
  void update(float delta) {
    paddle_.update(delta);
    ball_.update(delta);

    if (ball_.collides(Rectangle{paddle_.x, paddle_.y, (float)paddle_.width, (float)paddle_.height})) {
      // ball_.y -= 8;
      ball_.dy = -ball_.dy;
      gSound.play("paddle-hit");
    }

    for(auto& b : bricks_) {
      if(b.inPlay && ball_.collides(Rectangle{b.x, b.y, (float)b.width, (float)b.height})) {
        b.hit();
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
};


} //end namespace breadout

void LoadRes() {
  breakout::gb_texture = LoadTexture("../assets/background.png");
  breakout::main_texture = LoadTexture("../assets/breakout.png");
  breakout::GSprites["paddles"] = GenerateQuadsPaddles();
  breakout::GSprites["balls"] = GenerateQuadsBalls();
  breakout::GSprites["bricks"] = GenerateQuads(breakout::main_texture, 32, 16);
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
  std::map<std::string, breakout::BaseState*> initialState = { {"start", &startState}, {"play", &playState} };
  breakout::stateMachine.SetStates(initialState);
  breakout::stateMachine.change("start");
  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
  #else
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
  #endif
  // while (!WindowShouldClose()) {
  //   float delta = GetFrameTime();
  //   breakout::stateMachine.update(delta);
  //   BeginDrawing();
	// 		ClearBackground(BLACK);


  //     DrawTexturePro(gb_texture, {0, 0, (float)gb_texture.width, (float)gb_texture.height}, {0, 0, (float)breakout::ScreenWidth + breakout::ScaleRatio + 2, (float)breakout::ScreenHeight + breakout::ScaleRatio + 2}, {0, 0}, 0, WHITE);
  //     breakout::stateMachine.render();

	// 		DrawFPS(10, 10);
	// 	EndDrawing();
  // }
  UnloadTexture(breakout::gb_texture);
  UnloadTexture(breakout::main_texture);
  breakout::gSound.unload();
  CloseWindow();
  return 0;
}