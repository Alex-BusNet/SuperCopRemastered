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
    rightWindowBound = (width / 70.0) + 1;
    windowWidth = width;
    qDebug() << "Ground: " << floorHeight;
}

LevelBase::~LevelBase()
{
    if(floor != NULL)
        delete floor;
}

void LevelBase::LoadLevel(int levelNum, GameView *view)
{
    // This is currently for Testing purposes.

    view->addRect(QRect(0, 0, 70*200, 5), QPen(Qt::transparent));

    int x = 0;
    for(int i = 0; i < 200; i++)
    {
        levelFloor.push_back(new BlockBase(GRASS, BLOCK));
        levelFloor.last()->SetPosition(x, floorHeight);

        floorItems.push_back(view->addPixmap(*(levelFloor.last()->GetTexture())));
        floorItems.last()->setPos(x, floorHeight);

        floorBBs.push_back(view->addRect(*(levelFloor.last()->GetBoundingBox())));
        x += 70;
    }

    obstacles.push_back(new BlockBase(GRASS, SHORT_PLATFORM));
    obstacles.last()->SetPosition(20 * 70, floorHeight - (70*4));
    obstacles.push_back(new BlockBase(GRASS, SHORT_WALL));
    obstacles.last()->SetPosition(29 * 70, floorHeight - (70*2));
    obstacles.push_back(new BlockBase(GRASS, TALL_DEEP_WALL));
    obstacles.last()->SetPosition(39 * 70, floorHeight - (70*4));

    foreach(BlockBase *bb, obstacles)
    {
        for(int i = 0; i < bb->GetXDim(); i++)
        {
            for(int j = 0; j < bb->GetYDim(); j++)
            {
                obstacleItems.push_back(view->addPixmap(*(bb->GetTexture())));
                obstacleItems.last()->setPos(bb->GetPosX() + (i * bb->GetSize().x), bb->GetPosY() + (j * bb->GetSize().y));
            }
        }

        obstacleBBs.push_back(view->addRect(*(bb->GetBoundingBox())));
    }

    enemies.push_back(new EnemyBase(18*70, floorHeight - 70, NORMAL));
    enemies.last()->SetBounds(18 * 70, 29 * 70);

    foreach(EnemyBase *eb, enemies)
    {
        enemyItems.push_back(view->addPixmap(*(eb->GetTexture())));
        enemyItems.last()->setPos(eb->GetPosX(), eb->GetPosY());
        enemyItems.last()->setScale(0.5);

        enemyBBs.push_back(view->addRect(*(eb->GetBoundingBox())));

        eb->SetGPixmapPtr(enemyItems.last());
        eb->SetGRectPtr(enemyBBs.last());
    }

//    leftObstacleBound = 0;
//    rightObstacleBound = 3;

//    leftEnemyBound = 0;
//    rightEnemyBound = 1;
}

void LevelBase::drawLevelBase(QPainter &painter)
{
//    for(int i = leftWindowBound; i < rightWindowBound; i++)
//    {
//        if(levelFloor.at(i) != NULL)
//            levelFloor.at(i)->DrawBlock(painter);
//    }

//    for(int i = leftObstacleBound; i < rightObstacleBound; i++)
//    {
//        if(obstacles.at(i) != NULL)
//            obstacles.at(i)->DrawBlock(painter);
//    }

//    for(int i = leftEnemyBound; i < rightEnemyBound; i++)
//    {
//        if(enemies.at(i) != NULL)
//            enemies.at(i)->DrawEnemy(painter);
//    }

    painter.drawText(15, 130, QString("LeftWindowBound: %1").arg(leftWindowBound));
    painter.drawText(15, 140, QString("RightWindowBound: %1").arg(rightWindowBound));
    painter.drawText(15, 150, QString("LeftObstacleBound: %1").arg(leftObstacleBound));
    painter.drawText(15, 160, QString("RightObstacleBound: %1").arg(rightObstacleBound));
    painter.drawText(15, 170, QString("LeftEnemyBound: %1").arg(leftEnemyBound));
    painter.drawText(15, 180, QString("RightEnemyBound: %1").arg(rightEnemyBound));
    painter.drawText(15, 200, QString("Enemy Viewport pos: %1, %2").arg(enemies.at(0)->GetGPixmapPtr()->pos().x()).arg(enemies.at(0)->GetGPixmapPtr()->pos().y()));

}

void LevelBase::UpdateLevel(int playerDir, PlayerState ps)
{
//    if(false/*ps == RUNNING_LEFT || ps == RUNNING_RIGHT*/)
//    {
//        // -----------------
//        //  FLOOR UPDATES
//        // -----------------

//        // Even though we aren't rendering all parts of the level,
//        // we still need to update them at the same time.
//        for(int i = 0; i < levelFloor.size(); i++)
//        {
//            // If the left most block rendered is the first block,
//            // don't update the floor blocks.
//            if(leftWindowBound == 0 && playerDir == WEST) { break; }

//            if(levelFloor.at(i) != NULL)
//                levelFloor.at(i)->UpdateBlock(playerDir);
//        }

//        if(playerDir == EAST)
//        {
//            if(levelFloor.at(leftWindowBound + 1) != NULL)
//            {
//                if(levelFloor.at(leftWindowBound)->GetRightEdge() < 0)
//                    leftWindowBound++;
//            }

//            if(levelFloor.at(rightWindowBound + 1) != NULL)
//            {
//                if(levelFloor.at(rightWindowBound)->GetPosX() > windowWidth)
//                    rightWindowBound++;
//            }
//        }
//        else if (playerDir == WEST)
//        {
//            // Check if we need to shift the left bound up one index
//            if(leftWindowBound > 0)
//            {
//                if(levelFloor.at(leftWindowBound - 1) != NULL)
//                {
//                    if(levelFloor.at(leftWindowBound - 1)->GetRightEdge() > 0)
//                        leftWindowBound--;
//                }

//                if(levelFloor.at(rightWindowBound) != NULL)
//                {
//                    // Check if we need to shift the right bound up one index
//                    if(levelFloor.at(rightWindowBound)->GetPosX() > windowWidth)
//                        rightWindowBound--;
//                }
//            }
//        }

//        // ------------------
//        // OBSTACLE UPDATES
//        // ------------------

//        for(int i = 0; i < obstacles.size(); i++)
//        {
//            if(leftWindowBound == 0 && playerDir == WEST) { break; }
//            if(obstacles.at(i) != NULL)
//                obstacles.at(i)->UpdateBlock(playerDir);
//        }

//        if(playerDir == EAST)
//        {
//            if(rightObstacleBound < obstacles.size() - 1)
//            {
//                if(leftObstacleBound < rightObstacleBound - 1)
//                {
//                    if(obstacles.at(leftObstacleBound)->GetRightEdge() < 0)
//                        leftObstacleBound++;
//                }

//                if(rightObstacleBound + 1 < obstacles.size() - 1)
//                {
//                    if(obstacles.at(rightObstacleBound + 1) != NULL &&
//                            ((obstacles.at(rightObstacleBound + 1)->GetPosX()) < windowWidth))
//                       rightObstacleBound++;
//                }
//            }

//        }
//        else if(playerDir == WEST)
//        {
//            if(leftObstacleBound > 0)
//            {
//                if(obstacles.at(leftObstacleBound - 1) != NULL &&
//                        (obstacles.at(leftObstacleBound - 1)->GetRightEdge() > 0))
//                    leftObstacleBound--;


//                if(rightObstacleBound != obstacles.size() - 1)
//                {
//                    if(obstacles.at(rightObstacleBound) != NULL &&
//                            ((obstacles.at(rightObstacleBound)->GetPosX()) > windowWidth))
//                        rightObstacleBound--;
//                }
//            }
//        }

//        // ---------------------
//        // ENEMY BOUNDS UPDATE
//        // ---------------------

//        if(playerDir == EAST)
//        {
//            if(leftEnemyBound < (rightEnemyBound - 1))
//            {
//                if((enemies.at(leftEnemyBound) != NULL) &&
//                        (enemies.at(leftEnemyBound)->GetRightEdge() < 0))
//                    leftEnemyBound++;

//                if(rightEnemyBound < enemies.size() - 1)
//                {
//                    if(enemies.at(rightEnemyBound + 1) != NULL &&
//                            (enemies.at(rightEnemyBound + 1)->GetPosX() < windowWidth))
//                        rightEnemyBound++;
//                }
//            }
//        }
//        else if(playerDir == WEST)
//        {
//            if(leftEnemyBound > 0)
//            {
//                if((enemies.at(leftEnemyBound - 1) != NULL) &&
//                        (enemies.at(leftEnemyBound - 1)->GetRightEdge() > 0))
//                    leftEnemyBound--;

//                if((enemies.at(rightEnemyBound) != NULL) &&
//                        (enemies.at(rightEnemyBound)->GetPosX() > windowWidth))
//                    rightEnemyBound--;
//            }
//        }
//    }

    // ---------------
    // ENEMY UPDATES
    // ---------------
    for(int i = 0; i < enemies.size(); i++)
    {
        if(enemies.at(i) != NULL)
            enemies.at(i)->UpdateEnemy(ps);
    }


}

int LevelBase::getGround()
{
    return floorHeight;
}//Draws the floor
