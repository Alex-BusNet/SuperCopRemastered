#ifndef DATATYPES_H
#define DATATYPES_H

struct Size { int x; int y; };

enum keyPressed {NONE = 0, RIGHT = 1, UP = 2, DOWN = 3, LEFT = 4, PAUSE = 5};

enum Direction {WEST = -1, STAND = 0, EAST = 1};

enum LevelType {GRASS, INDUSTRIAL, MEDIEVIL, CAVE, NO_LEVEL_TYPE};

enum ItemType {DONUT, ONE_UP};

enum EnemyType {NORMAL, FAST, JUMPER, TURRET};

enum BlockType {
    BLOCK,           // 1x1 Block
    SHORT_PLATFORM,  // 3 Blocks long
    MEDIUM_PLATFORM, // 4 Blocks long
    LONG_PLATFORM,   // 5 Blocks long
    SHORT_WALL,      // 2x2
    TALL_WALL,       // 4x2
    SHORT_DEEP_WALL, // 2x4
    TALL_DEEP_WALL,  // 4x4
    SHORT_STAIR,     // 3x3
    TALL_STAIR,      // 6x6
    GOAL,            // Flag
    BONUS            // 1x1 Block (Special)
};
#endif // DATATYPES_H
