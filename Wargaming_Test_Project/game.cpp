#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const int SPACE_BETWEEN_TILES = 8;
const int TILE_SIZE = 128;
const int TILE_ROW_COLUMN_COUNT = 5;
const int TILE_COUNT = 25;
const int WINDOW_WIDTH = 688;
const int DESELECTED_TILE = -1;
const int EMPTY_TILE = -1;
const int FORBIDDEN_MOVEMENT_LEFT = 0;
const int FORBIDDEN_MOVEMENT_RIGHT = 4;

const int COLUMN_X = 0;
const int COLUMN_Y = 2;
const int COLUMN_Z = 4;

enum TileType {
    Block,
    X,
    Y,
    Z
};

enum TileDirection {
    Up = -5,
    Down = 5,
    Left = -1,
    Right = 1
};

class Tile {
public:
    olc::Sprite *mainSprite = nullptr;
    olc::Decal *mainDecal = nullptr;
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

        if (mainSprite != nullptr) {
            mainDecal = new olc::Decal(mainSprite);
        }
    }
};

class PlayGround {
public:
    olc::Sprite *mainSprite;
    olc::Sprite *selectedTileSprite;
    olc::Decal *mainDecal;
    olc::Decal *selectedTileDecal;
    Tile **tiles;

    PlayGround() {
        mainSprite = new olc::Sprite("resources\\playingGround.png");
        mainDecal = new olc::Decal(mainSprite);
        selectedTileSprite = new olc::Sprite("resources\\selection.png");
        selectedTileDecal = new olc::Decal(selectedTileSprite);
        tiles = new Tile*[TILE_COUNT];
        emptyPlayGround();
    }

    void emptyPlayGround() {
        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            tiles[tileNumber] = nullptr;
        }
    }
};

class TheGame : public olc::PixelGameEngine {
    const int select = olc::Mouse::LEFT;
    const int deselect = olc::Mouse::RIGHT;
    PlayGround *playGround;
    Tile *selectedTile;
    int selectedTileNumber;
    int xCoords[TILE_COUNT];
    int yCoords[TILE_COUNT];
public:
    bool OnUserCreate() override {
        SetPixelMode(olc::Pixel::ALPHA);
        setTileCoords();
        deselectTile();
        playGround = new PlayGround();
        sAppName = "Test Project";
        tempHardcodedPlayGroundFill();
        return true;
    }

    bool OnUserUpdate(float elapsedTime) override {
        drawGame();

        if (GetKey(olc::ESCAPE).bPressed) {
            return false;
        }

        if (GetMouse(deselect).bPressed) {
            deselectTile();
        }

        if (GetMouse(select).bPressed) {
            if (tileSelected()) {
                Tile *targetTile = nullptr;
                int targetTileNumber = EMPTY_TILE;
                getTile(GetMouseX(), GetMouseY(), targetTile, targetTileNumber);
                if (targetTile == nullptr && adjacentTileCheck(targetTileNumber)) {
                    playGround->tiles[targetTileNumber] = playGround->tiles[selectedTileNumber];
                    playGround->tiles[selectedTileNumber] = nullptr;
                    selectedTileNumber = targetTileNumber;
                    if (winCheck()) {
                        std::cout << "You have won!" << std::endl;
                        std::cout << "Press Escape to exit" << std::endl;
                        deselectTile();
                        playGround->emptyPlayGround();
                    }
                }
            }
            else {
                Tile *targetTile = nullptr;
                int targetTileNumber = EMPTY_TILE;
                getTile(GetMouseX(), GetMouseY(), targetTile, targetTileNumber);
                if (targetTile != nullptr) {
                    if (targetTile->type != TileType::Block) {
                        selectedTile = targetTile;
                        selectedTileNumber = targetTileNumber;
                    }
                }
            }
        }
        return true;
    }

    bool OnUserDestroy() {
        return true;
    }

    void setTileCoords() {
        int current_tile_index = 0;

        for (int yTile = 0; yTile < TILE_ROW_COLUMN_COUNT; yTile++) {
            for (int xTile = 0; xTile < TILE_ROW_COLUMN_COUNT; xTile++) {
                current_tile_index = xTile + yTile * TILE_ROW_COLUMN_COUNT;
                xCoords[current_tile_index] = SPACE_BETWEEN_TILES * (1 + xTile) + TILE_SIZE * xTile; // *
                yCoords[current_tile_index] = SPACE_BETWEEN_TILES * (1 + yTile) + TILE_SIZE * yTile; // *
            }
        }
    }

    void drawGame() {
        DrawDecal({0, 0}, playGround->mainDecal);

        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            if (playGround->tiles[tileNumber] != nullptr) {
                DrawDecal(
                    {(float)xCoords[tileNumber], (float)yCoords[tileNumber]},
                    playGround->tiles[tileNumber]->mainDecal
                );
            }
        }

        if (tileSelected()) {
            DrawDecal(
                {(float)xCoords[selectedTileNumber], (float)yCoords[selectedTileNumber]},
                playGround->selectedTileDecal
            );
        }
    }

    bool winCheck() {
        return columnMatchCheck(TileType::X, COLUMN_X) &&
               columnMatchCheck(TileType::Y, COLUMN_Y) &&
               columnMatchCheck(TileType::Z, COLUMN_Z);
    }

    bool columnMatchCheck(TileType type, int columnNumber) {
        for (int columnTile = columnNumber; columnTile < TILE_COUNT; columnTile += TILE_ROW_COLUMN_COUNT) {
            if (playGround->tiles[columnTile] == nullptr ||
                playGround->tiles[columnTile]->type != type) {
                return false;
            }
        }

        return true;
    }

    bool tileSelected() {
        return selectedTile != nullptr;
    }

    bool getTile(int xCoord, int yCoord, Tile *&resultTile, int &resultTileNumber) {
        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            if ((xCoords[tileNumber] <= xCoord && xCoord <= xCoords[tileNumber] + TILE_SIZE) &&
                (yCoords[tileNumber] <= yCoord && yCoord <= yCoords[tileNumber] + TILE_SIZE)) {
                if (playGround->tiles[tileNumber] == nullptr) {
                    resultTile = nullptr;
                    resultTileNumber = tileNumber;
                    return false;
                }

                resultTile = playGround->tiles[tileNumber];
                resultTileNumber = tileNumber;
                return true;
            }
        }
        return false;
    }

    void deselectTile() {
        selectedTile = nullptr;
        selectedTileNumber = DESELECTED_TILE;
    }

    bool adjacentTileCheck(int targetTileNumber) {
        if (selectedTileNumber == targetTileNumber + TileDirection::Left) {
            return forbiddenMovementCheck(targetTileNumber, TileDirection::Left);
        }

        if (selectedTileNumber == targetTileNumber + TileDirection::Right) {
            return forbiddenMovementCheck(targetTileNumber, TileDirection::Right);
        }

        return selectedTileNumber == targetTileNumber + TileDirection::Up ||
               selectedTileNumber == targetTileNumber + TileDirection::Down;
    }

    bool forbiddenMovementCheck(int targetTileNumber, TileDirection direction) {
        if (direction == TileDirection::Left) {
            return !(targetTileNumber % TILE_ROW_COLUMN_COUNT == FORBIDDEN_MOVEMENT_LEFT);
        }

        if (direction == TileDirection::Right) {
            return !(targetTileNumber % TILE_ROW_COLUMN_COUNT == FORBIDDEN_MOVEMENT_RIGHT);
        }
        
        return true;
    }

    void tempHardcodedPlayGroundFill() {
        playGround->tiles[0] = new Tile(TileType::X);
        playGround->tiles[1] = new Tile(TileType::Block);
        playGround->tiles[2] = new Tile(TileType::Y);
        playGround->tiles[3] = new Tile(TileType::Block);
        playGround->tiles[4] = new Tile(TileType::Z);
        playGround->tiles[5] = new Tile(TileType::Y);
        playGround->tiles[7] = new Tile(TileType::X);
        playGround->tiles[9] = new Tile(TileType::Z);
        playGround->tiles[10] = new Tile(TileType::Z);
        playGround->tiles[11] = new Tile(TileType::Block);
        playGround->tiles[12] = new Tile(TileType::Z);
        playGround->tiles[13] = new Tile(TileType::Block);
        playGround->tiles[14] = new Tile(TileType::Y);
        playGround->tiles[15] = new Tile(TileType::Y);
        playGround->tiles[17] = new Tile(TileType::X);
        playGround->tiles[19] = new Tile(TileType::Y);
        playGround->tiles[20] = new Tile(TileType::Z);
        playGround->tiles[21] = new Tile(TileType::Block);
        playGround->tiles[22] = new Tile(TileType::X);
        playGround->tiles[23] = new Tile(TileType::Block);
        playGround->tiles[24] = new Tile(TileType::X);
    }
};

int main() {
    TheGame game;

    if (game.Construct(WINDOW_WIDTH, WINDOW_WIDTH, 1, 1)) { // *
        game.Start();
    }

    return 0;
}