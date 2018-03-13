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
    windowWidth = width;
    qDebug() << "Ground: " << floorHeight;
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

    painter.drawText(10, 130, QString("LeftWindowBound: %1").arg(leftWindowBound));
    painter.drawText(10, 140, QString("RightWindowBound: %1").arg(rightWindowBound));
    painter.drawText(10, 150, QString("LeftObstacleBound: %1").arg(leftObstacleBound));
    painter.drawText(10, 160, QString("RightObstacleBound: %1").arg(rightObstacleBound));
    painter.drawText(10, 170, QString("LeftEnemyBound: %1").arg(leftEnemyBound));
    painter.drawText(10, 180, QString("RightEnemyBound: %1").arg(rightEnemyBound));
}

void LevelBase::UpdateLevel(int playerDir, PlayerState ps)
{
    if(ps == RUNNING_LEFT || ps == RUNNING_RIGHT)
    {
        // -----------------
        //  FLOOR UPDATES
        // -----------------

        // Even though we aren't rendering all parts of the level,
        // we still need to update them at the same time.
        for(int i = 0; i < levelFloor.size(); i++)
        {
            // If the left most block rendered is the first block,
            // don't update the floor blocks.
            if(leftWindowBound == 0 && playerDir == WEST) { break; }

            if(levelFloor.at(i) != NULL)
                levelFloor.at(i)->UpdateBlock(playerDir);
        }

        if(playerDir == EAST)
        {
            if(levelFloor.at(leftWindowBound + 1) != NULL)
            {
                if(levelFloor.at(leftWindowBound)->GetRightEdge() < 0)
                    leftWindowBound++;
            }

            if(levelFloor.at(rightWindowBound + 1) != NULL)
            {
                if(levelFloor.at(rightWindowBound)->GetPosX() > windowWidth)
                    rightWindowBound++;
            }
        }
        else if (playerDir == WEST)
        {
            // Check if we need to shift the left bound up one index
            if(leftWindowBound > 0)
            {
                if(levelFloor.at(leftWindowBound - 1) != NULL)
                {
                    if(levelFloor.at(leftWindowBound - 1)->GetRightEdge() > 0)
                        leftWindowBound--;
                }

                if(levelFloor.at(rightWindowBound) != NULL)
                {
                    // Check if we need to shift the right bound up one index
                    if(levelFloor.at(rightWindowBound)->GetPosX() > windowWidth)
                        rightWindowBound--;
                }
            }
        }

        // ------------------
        // OBSTACLE UPDATES
        // ------------------

        for(int i = 0; i < obstacles.size(); i++)
        {
            if(leftWindowBound == 0 && playerDir == WEST) { break; }
            if(obstacles.at(i) != NULL)
                obstacles.at(i)->UpdateBlock(playerDir);
        }

        if(playerDir == EAST)
        {
            if(rightObstacleBound < obstacles.size() - 1)
            {
                if(leftObstacleBound < rightObstacleBound - 1)
                {
                    if(obstacles.at(leftObstacleBound)->GetPosX() < 0)
                        leftObstacleBound++;
                }

                if(rightObstacleBound + 1 < obstacles.size() - 1)
                {
                    if(obstacles.at(rightObstacleBound + 1) != NULL &&
                            ((obstacles.at(rightObstacleBound + 1)->GetRightEdge()) < windowWidth))
                       rightObstacleBound++;
                }
            }

        }
        else if(playerDir == WEST)
        {
            if(leftObstacleBound > 0)
            {
                if(obstacles.at(leftObstacleBound - 1) != NULL &&
                        (obstacles.at(leftObstacleBound - 1)->GetRightEdge() > 0))
                    leftObstacleBound--;


                if(rightObstacleBound != obstacles.size() - 1)
                {
                    if(obstacles.at(rightObstacleBound) != NULL &&
                            ((obstacles.at(rightObstacleBound)->GetPosX()) > windowWidth))
                        rightObstacleBound--;
                }
            }
        }

        // ---------------
        // ENEMY UPDATES
        // ---------------
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

int LevelBase::getGround()
{
    return floorHeight;
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
