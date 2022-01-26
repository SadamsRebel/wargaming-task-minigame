#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

enum TileType {
    Block,
    X,
    Y,
    Z
};

enum MovementDir {
    Up = -5,
    Down = 5,
    Left = -1,
    Right = 1
};

class Tile {
public:
    olc::Sprite mainSprite;
    TileType type;
    Tile() {}
    Tile(TileType type) {
        this->type = type;
        switch(type) {
        case TileType::Block:
            break;
        case TileType::X:
            break;
        case TileType::Y:
            break;
        case TileType::Z:
            break;
        }
    }
};

class PlayGround {
public:
    olc::Sprite mainSprite;
    Tile *tiles;
    PlayGround() {
        tiles = new Tile[25]{};
    }
};

class TheGame : public olc::PixelGameEngine {
    Tile *selectedTile;
    PlayGround *playGround;
public:
    bool OnUserCreate() override {
        selectedTile = nullptr;
        playGround = new PlayGround();
        sAppName = "Test Project";
    }
    bool OnUserUpdate(float elapsedTime) override {

    }
    bool OnUserDestroy() {

    }
};

int main() {
    TheGame game;
    if (game.Construct(720, 700, 1, 1)) {
        game.Start();
    }
    return 0;
}