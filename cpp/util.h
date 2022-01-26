#pragma once
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "raylib.h"

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

// auto GenerateQuadsBricks() {
//     return GenerateQuads(atlas, 32, 16), 1, 21)
// }