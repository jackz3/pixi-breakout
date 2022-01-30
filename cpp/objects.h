#pragma once
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
#include "util.h"

namespace breakout {

class Brick {
public:
  float x;
  float y;
  int width = 32;
  int height = 16;
  bool inPlay = true;
  int tier = 0;
  int color = 1;
  Brick(float x, float y) : x{x}, y{y} {}
  void hit() {
    if(tier > 0) {
        if(color == 1) {
            tier--;
            color = 5;
        } else {
          color--;
        }
    } else {
        // -- if we're in the first tier and the base color, remove brick from play
        if(color == 1) {
            inPlay = false;
        } else {
          color--;
        }
    }
    // -- play a second layer sound if the brick is destroyed
    if(inPlay) {
      gSound.play("brick-hit-2");
    } else {
      gSound.play("brick-hit-1");
    }
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
    // -- randomly choose the number of columns, ensuring odd
    int numCols = GetRandomValue(7, 13);
    numCols = numCols % 2 == 0 ? (numCols + 1) : numCols;
    // -- highest possible spawned brick color in this level; ensure we don't go above 3
    int highestTier = std::min(3, (int)floor(level / 5));
    // -- highest color of the highest tier
    int highestColor = std::min(5, level % 5 + 3);

    for(int y = 1; y <= numRows; y++) {
      // -- whether we want to enable skipping for this row
      int skipPattern = GetRandomValue(1, 2) == 1;
        // -- whether we want to enable alternating colors for this row
      int alternatePattern = GetRandomValue(1, 2) == 1;
      // -- choose two colors to alternate between
      int alternateColor1 = GetRandomValue(1, highestColor);
      int alternateColor2 = GetRandomValue(1, highestColor);
      int alternateTier1 = GetRandomValue(0, highestTier);
      int alternateTier2 = GetRandomValue(0, highestTier);
      // -- used only when we want to skip a block, for skip pattern
      int skipFlag = GetRandomValue(1, 2) == 1;
      // -- used only when we want to alternate a block, for alternate pattern
      int alternateFlag = GetRandomValue(1, 2) == 1;
      // -- solid color we'll use if we're not skipping or alternating
      int solidColor = GetRandomValue(1, highestColor);
      int solidTier = GetRandomValue(0, highestTier);
      for(int x = 0; x < numCols; x++) {
            // -- if skipping is turned on and we're on a skip iteration...
            if(skipPattern && skipFlag) {
                // -- turn skipping off for the next iteration
                skipFlag = !skipFlag;
                // -- Lua doesn't have a continue statement, so this is the workaround
                continue;
            } else {
                // -- flip the flag to true on an iteration we don't use it
                skipFlag = !skipFlag;
            }
            auto b = Brick(x * 32 +8 + (13 - numCols) * 16, y * 16);
                // -- x-coordinate
                // (x-1)                   -- decrement x by 1 because tables are 1-indexed, coords are 0
                // * 32                    -- multiply by 32, the brick width
                // + 8                     -- the screen should have 8 pixels of padding; we can fit 13 cols + 16 pixels total
                // + (13 - numCols) * 16,  -- left-side padding for when there are fewer than 13 columns
                // -- y-coordinate
                // y * 16                  -- just use y * 16, since we need top padding anyway
            // -- if we're alternating, figure out which color/tier we're on
            if(alternatePattern && alternateFlag) {
                b.color = alternateColor1;
                b.tier = alternateTier1;
                alternateFlag = !alternateFlag;
            } else {
                b.color = alternateColor2;
                b.tier = alternateTier2;
                alternateFlag = !alternateFlag;
            }
            // -- if not alternating and we made it here, use the solid color/tier
            if(!alternatePattern) {
                b.color = solidColor;
                b.tier = solidTier;
            }
          bricks.push_back(b);
        }
    } 
    return bricks;
}
}