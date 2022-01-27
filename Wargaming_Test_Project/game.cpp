#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const int SPACE_BETWEEN_TILES = 8;
const int TILE_SIZE = 128;
const int TILE_ROW_COLUMN_COUNT = 5;
const int TILE_COUNT = 25;

enum TileType {
    Empty,
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
    olc::Sprite *mainSprite;
    TileType type;
    Tile(TileType type) {
        this->type = type;
        switch(type) {
        case TileType::Block:
            mainSprite = new olc::Sprite("resources\\block.png");
            break;
        case TileType::X:
            mainSprite = new olc::Sprite("resources\\xTile.png");
            break;
        case TileType::Y:
            mainSprite = new olc::Sprite("resources\\yTile.png");
            break;
        case TileType::Z:
            mainSprite = new olc::Sprite("resources\\zTile.png");
            break;
        default:
            break;
        }
    }
};

class PlayGround {
public:
    olc::Sprite *mainSprite;
    Tile **tiles;
    PlayGround() {
        mainSprite = new olc::Sprite("resources\\playingGround.png");
        tiles = new Tile*[TILE_COUNT];
    }
};

class TheGame : public olc::PixelGameEngine {
    Tile *selectedTile;
    PlayGround *playGround;
    int xCoords[TILE_COUNT];
    int yCoords[TILE_COUNT];
public:
    bool OnUserCreate() override {
        setTileCoords();
        selectedTile = nullptr;
        playGround = new PlayGround();
        sAppName = "Test Project";
        for (int cntr = 0; cntr < TILE_COUNT; cntr++) {
            playGround->tiles[cntr] = new Tile(TileType::Block);
        }
        return true;
    }
    bool OnUserUpdate(float elapsedTime) override {
        drawGame();
        return true;
    }
    bool OnUserDestroy() {
        return true;
    }
    void setTileCoords() {
        int index = 0;
        for (int yTile = 0; yTile < TILE_ROW_COLUMN_COUNT; yTile++) {
            for (int xTile = 0; xTile < TILE_ROW_COLUMN_COUNT; xTile++) {
                index = xTile + yTile * TILE_ROW_COLUMN_COUNT;
                xCoords[index] = SPACE_BETWEEN_TILES * (1 + xTile) + TILE_SIZE * xTile; // *
                yCoords[index] = SPACE_BETWEEN_TILES * (1 + yTile) + TILE_SIZE * yTile; // *
            }
        }
    }
    void drawGame() {
        DrawSprite(0, 0, playGround->mainSprite);
        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            DrawSprite(xCoords[tileNumber], yCoords[tileNumber], playGround->tiles[tileNumber]->mainSprite);
        }
    }
};

int main() {
    TheGame game;
    if (game.Construct(688, 688, 1, 1)) {
        game.Start();
    }
    return 0;
}