#ifndef DATATYPES_H
#define DATATYPES_H
#include <QString>

struct Size { int x; int y; };

enum keyPressed {NONE = 0, RIGHT = 1, UP = 2, DOWN = 3, LEFT = 4, PAUSE = 5};

enum Inputs {MOVE_RIGHT = 0, MOVE_LEFT = 1, SPRINT = 2, JUMP = 3, STOP = 4};

enum Direction {WEST = -1, STAND = 0, EAST = 1};

enum LevelType {GRASS, INDUSTRIAL, MEDIEVIL, CAVE, NO_LEVEL_TYPE};

enum ItemType {DONUT, ONE_UP, NO_ITEM_TYPE = -1};

enum EnemyType {NORMAL, FAST, JUMPER, TURRET};

enum PlayerState {IDLE, RUNNING_RIGHT, JUMPING, SLIDING, RUNNING_LEFT, FALLING, PAUSED, VICTORY};

enum BlockType {
    BLOCK               = 0,
    BLOCK_EDGE_TOP      = 1,
    WALL_CORNER_LEFT    = 2,
    WALL_CORNER_RIGHT   = 3,
    WALL_SIDE_LEFT      = 4,
    WALL_SIDE_RIGHT     = 5,
    PLATFORM_LEFT       = 6,
    PLATFORM_RIGHT      = 7,
    INTERNAL_BLOCK      = 8,
    GOAL                = 9,
    GOAL_BASE           = 10,
    GOAL_MIDDLE         = 11,
    BONUS               = 12,
    FLOOR_COVERED_CORNER_LEFT = 13,
    FLOOR_COVERED_CORNER_RIGHT = 14,
    GAP_BLOCK           = 15,
    NO_BLOCK_TYPE       = -1
};
#endif // DATATYPES_H
