#include "levelbase.h"
#include <QDebug>

LevelBase::LevelBase()
{
    floorHeight = 100;
}

LevelBase::LevelBase(int width, int height)
{
    floorHeight = height - 100;
    imageSize = Size{70,70};
    leftWindowBound = 0;
    rightWindowBound = (width / 35.0) + 1;
}

LevelBase::~LevelBase()
{
    if(floor != NULL)
        delete floor;
}

void LevelBase::LoadLevel(int levelNum)
{
    // This is currently for Testing purposes.

    int x = 0;
    for(int i = 0; i < 500; i++)
    {
        levelFloor.push_back(new BlockBase(GRASS, BLOCK));
        levelFloor.last()->SetPosition(x, floorHeight);
        x += 35;
    }

    obstacles.push_back(new BlockBase(GRASS, SHORT_PLATFORM));
    obstacles.last()->SetPosition(280, floorHeight - 105);
    obstacles.push_back(new BlockBase(GRASS, SHORT_WALL));
    obstacles.last()->SetPosition(430, floorHeight - 70);

    enemies.push_back(new EnemyBase(530, floorHeight - 35, NORMAL));

    leftObstacleBound = 0;
    rightObstacleBound = 2;

    leftEnemyBound = 0;
    rightEnemyBound = 1;
}

void LevelBase::drawLevelBase(QPainter &painter)
{
    for(int i = leftWindowBound; i < rightWindowBound; i++)
    {
        if(levelFloor.at(i) != NULL)
            levelFloor.at(i)->DrawBlock(painter);
    }

    for(int i = leftObstacleBound; i < rightObstacleBound; i++)
    {
        if(obstacles.at(i) != NULL)
            obstacles.at(i)->DrawBlock(painter);
    }

    for(int i = leftEnemyBound; i < rightEnemyBound; i++)
    {
        if(enemies.at(i) != NULL)
            enemies.at(i)->DrawEnemy(painter);
    }
}

void LevelBase::ShiftBoundsReverse()
{
    leftWindowBound--;
    rightWindowBound--;

    if(leftWindowBound < 0)
    {
        leftWindowBound = 0;
        rightWindowBound++;
    }
}

void LevelBase::ShiftBoundsForward()
{
    leftWindowBound++;
    rightWindowBound++;

    if(rightWindowBound > (levelFloor.size() - 1))
    {
        rightWindowBound = levelFloor.size() - 1;
        leftWindowBound--;
    }

}//Draws the floor
