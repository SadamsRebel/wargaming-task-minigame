#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <map>

const int PIXEL_SIZE = 1;
const int FIRST_BORDER = 1;
const int BORDER_SIZE = 8;
const int TILE_SIZE = 128;
const int TILE_ROW_COLUMN_COUNT = 5;
const int TILE_COUNT = 25;
const int PLAYGROUND_IMAGE_COUNT = 1;
const int SELECTION_IMAGE_COUNT = 1;
const int WINDOW_HEIGTH = 688;
const int WINDOW_WIDTH = 688;
const int DESELECTED_TILE = -1;
const int EMPTY_TILE = -1;
const int TILE_MOVEMENT_LEFT = -1;
const int TILE_MOVEMENT_RIGHT = 1;
const int TILE_MOVEMENT_UP = -TILE_ROW_COLUMN_COUNT;
const int TILE_MOVEMENT_DOWN = TILE_ROW_COLUMN_COUNT;
const int FORBIDDEN_MOVEMENT_LEFT = 0;
const int FORBIDDEN_MOVEMENT_RIGHT = 4;

const int COLUMN_GLYPH1 = 0;
const int COLUMN_GLYPH2 = 2;
const int COLUMN_GLYPH3 = 4;

const int SELECT = olc::Mouse::LEFT;
const int DESELECT = olc::Mouse::RIGHT;
const olc::Key EXIT_GAME = olc::ESCAPE;

const std::string GAME_NAME = "Master of Glyphs";

const std::string PLAYGROUND_IMAGE = "resources\\playingGround.png";
const std::string SELECTION_IMAGE = "resources\\selection.png";
const std::string EMPTY_IMAGE = "resources\\empty.png";
const std::string BLOCK_IMAGE = "resources\\block.png";
const std::string GLYPH_1_IMAGE = "resources\\glyph1.png";
const std::string GLYPH_2_IMAGE = "resources\\glyph2.png";
const std::string GLYPH_3_IMAGE = "resources\\glyph3.png";

enum TileType {
    EMPTY,
    BLOCK,
    GLYPH_1,
    GLYPH_2,
    GLYPH_3
};

std::map<TileType, std::string> tileImages = {
    {TileType::EMPTY, EMPTY_IMAGE},
    {TileType::BLOCK, BLOCK_IMAGE},
    {TileType::GLYPH_1, GLYPH_1_IMAGE}, // *
    {TileType::GLYPH_2, GLYPH_2_IMAGE},
    {TileType::GLYPH_3, GLYPH_3_IMAGE}
};

enum TileMoveDirection {
    UP = TILE_MOVEMENT_UP,
    DOWN = TILE_MOVEMENT_DOWN,
    LEFT = TILE_MOVEMENT_LEFT,
    RIGHT = TILE_MOVEMENT_RIGHT
};

class Location {
public:
    int xCoord;
    int yCoord;

    Location() {
        xCoord = 0;
        yCoord = 0;
    }

    Location(int x, int y) {
        xCoord = x;
        yCoord = y;
    }

    ~Location() {}

    olc::vf2d getOlcCoordsFloat() {
        return olc::vf2d{(float)xCoord, (float)yCoord};
    }

    void setLocation(Location *newLocation) {
        this->xCoord = newLocation->xCoord;
        this->yCoord = newLocation->yCoord;
    }
};

class MouseClick {
public:
    Location *location;
    int buttonPressed;

    MouseClick(int xCoord, int yCoord, int buttonPressed) {
        location = new Location(xCoord, yCoord);
        this->buttonPressed = buttonPressed;
    }

    ~MouseClick() {
        delete location;
    }
};

class DrawObject {
public:
    olc::Sprite *sprite;
    olc::Decal *decal;

    DrawObject(std::string image) {
        sprite = new olc::Sprite(image);
        decal = new olc::Decal(sprite);
    }

    ~DrawObject() {
        delete decal;
        delete sprite;
    }
};

class DrawPack {
public:
    DrawObject *drawObject;
    Location *location;

    DrawPack(DrawObject *drawObject, Location *location) {
        this->drawObject = drawObject;
        this->location = location;
    }

    ~DrawPack() {
        drawObject = nullptr;
        location = nullptr;
    }
};

class Tile {
public:
    TileType type;

    Location *location;
    DrawObject *appearance;

    Tile(TileType type, Location *location = new Location(0, 0)) {
        this->location = location;
        this->type = type;

        appearance = new DrawObject(tileImages[type]);
    }

    ~Tile() {
        delete location;
        delete appearance;
    }

    void changeLocation(Location *newLocation) {
        delete location;
        location = newLocation;
    }

    void swapLocation(Tile *targetTile) {
        Location *tempLocation = targetTile->location;
        targetTile->location = this->location;
        this->location = tempLocation;
    }

    bool isGlyph() {
        return type == TileType::GLYPH_1 ||
               type == TileType::GLYPH_2 ||
               type == TileType::GLYPH_3;
    }
};

class PlayGround {
public:
    Location *location;
    DrawObject *appearance;
    DrawObject *selection;
    Tile *selectedTile;
    int selectedTileNumber;
    Tile **tiles;

    PlayGround(Location *location = new Location(0, 0)) {
        this->location = location;
        appearance = new DrawObject(PLAYGROUND_IMAGE);
        selection = new DrawObject(SELECTION_IMAGE);
        tiles = new Tile*[TILE_COUNT];
        selectedTile = nullptr;

        tempHardcodedPlayGroundFill();
        setTileCoords();
    }

    ~PlayGround() {
        emptyPlayGround();
        delete appearance;
        delete selection;
        delete tiles;
    }

    void emptyPlayGround() {
        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            delete tiles[tileNumber];
        }
    }

    bool getTile(Location *clickLocation, Tile *&targetTile, int &targetTileNumber) {
        for (int tileNumber = 0; tileNumber < TILE_COUNT; tileNumber++) {
            if (checkCollision(clickLocation, tiles[tileNumber]->location)) {
                targetTile = tiles[tileNumber];
                targetTileNumber = tileNumber;

                return true;
            }
        }
        targetTile = nullptr;
        targetTileNumber = DESELECTED_TILE;

        return false;
    }

    bool checkAxisCollision(int targetCoord, int probableLocation) {
        return probableLocation <= targetCoord && targetCoord <= probableLocation + TILE_SIZE;
    }

    bool checkCollision(Location *clickLocation, Location *tileLocation) {
        return checkAxisCollision(clickLocation->xCoord, tileLocation->xCoord) &&
               checkAxisCollision(clickLocation->yCoord, tileLocation->yCoord);
    }

    void setTileCoords() {
        int currentTileIndex = 0;

        for (int placeInColumn = 0; placeInColumn < TILE_ROW_COLUMN_COUNT; placeInColumn++) {
            for (int placeInRow = 0; placeInRow < TILE_ROW_COLUMN_COUNT; placeInRow++) {
                currentTileIndex = getTileIndex(placeInRow, placeInColumn);

                tiles[currentTileIndex]->changeLocation(new Location(
                    calculateCoord(placeInRow, location->xCoord),
                    calculateCoord(placeInColumn, location->yCoord)
                ));
            }
        }
    }

    int getTileIndex(int placeInRow, int placeInColumn) {
        return placeInRow + placeInColumn * TILE_ROW_COLUMN_COUNT;
    }

    int calculateCoord(int placeInRowOrColumn, int playGroundCorrespondence) {
        return (BORDER_SIZE * (FIRST_BORDER + placeInRowOrColumn) + TILE_SIZE * placeInRowOrColumn) + playGroundCorrespondence;
    }

    bool winCheck() {
        return columnMatchCheck(TileType::GLYPH_1, COLUMN_GLYPH1) &&
               columnMatchCheck(TileType::GLYPH_2, COLUMN_GLYPH2) &&
               columnMatchCheck(TileType::GLYPH_3, COLUMN_GLYPH3);
    }

    bool columnMatchCheck(TileType type, int columnNumber) {
        for (int columnTile = columnNumber; columnTile < TILE_COUNT; columnTile += TILE_ROW_COLUMN_COUNT) {
            if (tiles[columnTile]->type != type) {
                return false;
            }
        }

        return true;
    }

    bool isTileSelected() {
        return selectedTile != nullptr;
    }

    void deselectTile() {
        selectedTile = nullptr;
        selectedTileNumber = DESELECTED_TILE;
    }

    bool adjacentTileCheck(int targetTileNumber) {
        if (selectedTileNumber == targetTileNumber + TileMoveDirection::LEFT) {
            return forbiddenMovementCheck(targetTileNumber, TileMoveDirection::LEFT);
        }

        if (selectedTileNumber == targetTileNumber + TileMoveDirection::RIGHT) {
            return forbiddenMovementCheck(targetTileNumber, TileMoveDirection::RIGHT);
        }

        return selectedTileNumber == targetTileNumber + TileMoveDirection::UP ||
               selectedTileNumber == targetTileNumber + TileMoveDirection::DOWN;
    }

    bool forbiddenMovementCheck(int targetTileNumber, TileMoveDirection direction) {
        if (direction == TileMoveDirection::LEFT) {
            return calculateForbiddenMovement(targetTileNumber, FORBIDDEN_MOVEMENT_LEFT);
        }

        if (direction == TileMoveDirection::RIGHT) {
            return calculateForbiddenMovement(targetTileNumber, FORBIDDEN_MOVEMENT_RIGHT);
        }

        return true;
    }

    bool calculateForbiddenMovement(int target, int checkValue) {
        return !(target % TILE_ROW_COLUMN_COUNT == checkValue);
    }

    void processClick(MouseClick *click) {
        if (click->buttonPressed == DESELECT) {
            deselectTile();

            return;
        }

        Tile *targetTile;
        int targetTileNumber;
        getTile(click->location, targetTile, targetTileNumber);

        if (targetTile == nullptr) {
            return;
        }

        if (!isTileSelected() && targetTile->isGlyph()) {
            selectedTile = targetTile;
            selectedTileNumber = targetTileNumber;

            return;
        }

        if (targetTile->type == TileType::EMPTY && adjacentTileCheck(targetTileNumber)) {
            moveTile(targetTile, targetTileNumber);
        }
    }

    void moveTile(Tile *targetTile, int targetTileNumber) {
        tiles[selectedTileNumber] = targetTile;
        tiles[targetTileNumber] = selectedTile;

        selectedTile->swapLocation(targetTile);

        selectedTileNumber = targetTileNumber;
    }

    DrawPack **collectDrawObjects(int &itemCount) {
        itemCount = PLAYGROUND_IMAGE_COUNT + TILE_COUNT;
        
        if (isTileSelected()) {
            itemCount += SELECTION_IMAGE_COUNT;
        }

        DrawPack **container = new DrawPack*[itemCount];
        int itemIndex = 0;

        container[itemIndex] = new DrawPack(appearance, location);
        itemIndex++;

        for (int tileIndex = 0; tileIndex < TILE_COUNT; tileIndex++, itemIndex++) {
            container[itemIndex] = new DrawPack(tiles[tileIndex]->appearance, tiles[tileIndex]->location);
        }

        if (isTileSelected()) {
            container[itemIndex] = new DrawPack(selection, selectedTile->location);
        }

        return container;
    }

    void tempHardcodedPlayGroundFill() {
        tiles[0] = new Tile(TileType::GLYPH_1);
        tiles[1] = new Tile(TileType::BLOCK);
        tiles[2] = new Tile(TileType::GLYPH_2);
        tiles[3] = new Tile(TileType::BLOCK);
        tiles[4] = new Tile(TileType::GLYPH_3);
        tiles[5] = new Tile(TileType::GLYPH_2);
        tiles[6] = new Tile(TileType::EMPTY);
        tiles[7] = new Tile(TileType::GLYPH_1);
        tiles[8] = new Tile(TileType::EMPTY);
        tiles[9] = new Tile(TileType::GLYPH_3);
        tiles[10] = new Tile(TileType::GLYPH_3);
        tiles[11] = new Tile(TileType::BLOCK);
        tiles[12] = new Tile(TileType::GLYPH_3);
        tiles[13] = new Tile(TileType::BLOCK);
        tiles[14] = new Tile(TileType::GLYPH_2);
        tiles[15] = new Tile(TileType::GLYPH_2);
        tiles[16] = new Tile(TileType::EMPTY);
        tiles[17] = new Tile(TileType::GLYPH_1);
        tiles[18] = new Tile(TileType::EMPTY);
        tiles[19] = new Tile(TileType::GLYPH_2);
        tiles[20] = new Tile(TileType::GLYPH_3);
        tiles[21] = new Tile(TileType::BLOCK);
        tiles[22] = new Tile(TileType::GLYPH_1);
        tiles[23] = new Tile(TileType::BLOCK);
        tiles[24] = new Tile(TileType::GLYPH_1);
    }
};

class TheGame : public olc::PixelGameEngine {
    PlayGround *playGround;

    MouseClick *currentClick;
public:
    bool OnUserCreate() override {
        sAppName = GAME_NAME;
        SetPixelMode(olc::Pixel::ALPHA);

        playGround = new PlayGround();

        return true;
    }

    bool OnUserUpdate(float elapsedTime) override {
        drawGame();

        if (GetKey(EXIT_GAME).bPressed) {
            return false;
        }

        currentClick = caughtClick();

        if (currentClick != nullptr) {
            playGround->processClick(currentClick);
            delete currentClick;

            if (playGround->winCheck()) {
                return false;
            }
        }

        return true;
    }

    bool OnUserDestroy() override {
        delete playGround;

        return true;
    }

    MouseClick *caughtClick() {
        if (GetMouse(SELECT).bPressed) {
            return new MouseClick(GetMouseX(), GetMouseY(), SELECT);
        }

        if (GetMouse(DESELECT).bPressed) {
            return new MouseClick(GetMouseX(), GetMouseY(), DESELECT);
        }

        return nullptr;
    }

    void drawGame() {
        int itemCount = 0;
        DrawPack **toDraw = playGround->collectDrawObjects(itemCount);

        for (int itemIndex = 0; itemIndex < itemCount; itemIndex++) {
            DrawDecal(
                toDraw[itemIndex]->location->getOlcCoordsFloat(),
                toDraw[itemIndex]->drawObject->decal);
        }

        clearDrawPackArray(toDraw, itemCount);
    }

    void clearDrawPackArray(DrawPack **target, int size) {
        for (int index = 0; index < size; index++) {
            delete target[index];
        }

        delete target;
    }
};

int main() {
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    TheGame game;

    if (game.Construct(WINDOW_WIDTH, WINDOW_HEIGTH, PIXEL_SIZE, PIXEL_SIZE)) { // *
        game.Start();
    }

    return 0;
}