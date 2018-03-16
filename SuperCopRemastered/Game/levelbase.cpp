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
    updateStatus = false;
    qDebug() << "Ground: " << floorHeight;
}

LevelBase::~LevelBase()
{
//    if(levelFloor != NULL)
//        delete levelFloor;
}

void LevelBase::LoadLevel(int levelNum, GameView *view)
{

    view->addRect(QRect(0, 0, 70*200, 5), QPen(Qt::transparent));

    QFile levelData(QString("Assets/Level/World1_%1.json").arg(levelNum));

    if(levelData.open(QIODevice::ReadOnly))
    {
        QByteArray byteArr = levelData.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(byteArr);
        QJsonArray arr = doc.array();
        QJsonArray defArr;
        QJsonObject topObject, subObj;

        int lastXPos = 0;

        qDebug() << "Array size: " << arr.size();

        for(int i = 0; i < arr.size(); i++)
        {
            topObject = arr.at(i).toObject();
            if(topObject.contains("objecttype"))
            {
                int type = topObject["objecttype"].toInt();
                if(type == 1)
                {
                    // Floor definition
                    qDebug() << "Loading floor definition...";

                    defArr = topObject["blockdefinitions"].toArray();
                    for(int j = 0; j < defArr.size(); j++)
                    {
                        subObj = defArr.at(j).toObject();
                        BlockType bt = (BlockType)subObj["blocktype"].toInt();
                        LevelType lt = (LevelType)subObj["leveltype"].toInt();
                        int xPos = subObj["unitX"].toInt();
                        int yPos = subObj["unitY"].toInt();

                        levelFloor.push_back(new BlockBase(lt, bt));
                        levelFloor.last()->SetPosition(xPos * 70, floorHeight - (yPos * 70));

                        floorItems.push_back(view->addPixmap(*(levelFloor.last()->GetTexture())));
                        floorItems.last()->setPos(xPos * 70, floorHeight - (yPos * 70));

//                        floorBBs.push_back(view->addRect(*(levelFloor.last()->GetBoundingBox())));
                    }
                }
                else if (type == 2)
                {
                    // Obstacle definition
                    qDebug() << "Loading obstacle definition...";

                    defArr = topObject["blockdefinitions"].toArray();
                    for(int j = 0; j < defArr.size(); j++)
                    {
                        subObj = defArr.at(j).toObject();
                        BlockType bt = (BlockType)subObj["blocktype"].toInt();
                        LevelType lt = (LevelType)subObj["leveltype"].toInt();
                        ItemType it = (ItemType)subObj["itemtype"].toInt();
                        int xPos = subObj["unitX"].toInt();
                        int yPos = subObj["unitY"].toInt();

                        obstacles.push_back(new BlockBase(lt, bt));

                        // Obstacle is the player start location
                        if(it == 3)
                        {
                            playerStart.setX(xPos * 70);
                            playerStart.setY(floorHeight - (yPos * 93));
                            qDebug() << "Player start: " << playerStart;
                        }
                        else
                        {
                            obstacles.last()->SetPosition(xPos * 70, floorHeight - (yPos * 70));
                        }

                        lastXPos = xPos;
                    }

                    qDebug() << "Adding Obstacles to view...";
                    // Add the obstacles to the GraphicsView
                    foreach(BlockBase *bb, obstacles)
                    {
                        if(bb != NULL)
                        {
                            obstacleItems.push_back(view->addPixmap(*(bb->GetTexture())));
                            obstacleItems.last()->setPos(bb->GetPosX(), bb->GetPosY());
//                            view->addText(QString("%1").arg(obstacleItems.size() - 1))->setPos(obstacleItems.last()->x() + 28, obstacleItems.last()->y() + 28);

                            if(bb->GetBoundingBox() != NULL)
                            {
                                view->addRect(*bb->GetTopBoundingBox());
                                view->addRect(*bb->GetLeftBoundingBox());
                                view->addRect(*bb->GetRightBoundingBox());
                                view->addRect(*bb->GetBottomBoundingBox());
                            }
//                                obstacleBBs.push_back(view->addRect(*(bb->GetBoundingBox())));
                        }
                    }
                }
                else if(type == 3)
                {
                    // Enemy definition
                    qDebug() << "Loading enemy definition...";

                    defArr = topObject["blockdefinitions"].toArray();
                    for(int j = 0; j < defArr.size(); j++)
                    {
                        subObj = defArr.at(j).toObject();
                        EnemyType et = (EnemyType)subObj["enemytype"].toInt();
                        Direction dir = (Direction)subObj["direction"].toInt();
                        int xPos = subObj["unitX"].toInt();
                        int yPos = subObj["unitY"].toInt();
                        int lbound = subObj["leftbound"].toInt();
                        int rbound = subObj["rightbound"].toInt();

                        enemies.push_back(new EnemyBase(xPos * 70, floorHeight - (yPos * 60), et));
                        enemies.last()->SetDirection(dir);
                        enemies.last()->SetBounds(lbound * 70, rbound * 70);
                    }

                    foreach(EnemyBase *eb, enemies)
                    {
                        enemyItems.push_back(view->addPixmap(*(eb->GetTexture())));
                        enemyItems.last()->setPos(eb->GetPosX(), eb->GetPosY());
                        enemyItems.last()->setScale(0.5);

//                        enemyTextItems.push_back(view->addText(QString("%1").arg(enemyItems.size() - 1)));
//                        enemyTextItems.last()->setPos(enemyItems.last()->x() + 30, enemyItems.last()->y() + 2);

//                        if(eb->GetBoundingBox() != NULL)
//                        {
//                            enemyBBs.push_back(view->addRect(*(eb->GetBoundingBox())));
//                            enemyBBs.last()->setPos(0,0);
//                            enemyBBs.last()->setScale(0.5);

//                            enemyBBText.push_back(view->addText(QString("%1").arg(enemyBBs.size() - 1)));
//                            enemyBBText.last()->setPos(enemyBBs.last()->x() + 30, enemyBBs.last()->y() + 12);

//                            eb->SetGRectPtr(enemyBBs.last());
//                            eb->SetGRectText(enemyBBText.last());
//                        }

                        eb->SetGPixmapPtr(enemyItems.last());
//                        eb->SetGPixmapText(enemyTextItems.last());
                    }
                }
            }
            else
                qDebug() << "[objecttype] key not found.";
        }
    }
    else
    {
        qDebug() << "Failed to open: " << levelData.fileName();
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

//    painter.drawText(15, 130, QString("LeftWindowBound: %1").arg(leftWindowBound));
//    painter.drawText(15, 140, QString("RightWindowBound: %1").arg(rightWindowBound));
//    painter.drawText(15, 150, QString("LeftObstacleBound: %1").arg(leftObstacleBound));
//    painter.drawText(15, 160, QString("RightObstacleBound: %1").arg(rightObstacleBound));
//    painter.drawText(15, 170, QString("LeftEnemyBound: %1").arg(leftEnemyBound));
    painter.drawText(15, 180, QString("Player items collided: %1").arg(collidedItems));
    painter.drawText(15, 190, QString("Level Update: %1").arg(updateStatus));
//    painter.drawText(15, 200, QString("Enemy BB pos: %1, %2").arg(enemyBBs.first()->rect().x()).arg(enemyBBs.first()->rect().y()));
//    painter.drawText(15, 210, QString("Enemy Pixmap Pos: %1, %2").arg(enemyItems.first()->pos().x()).arg(enemyItems.first()->pos().y()));
//    painter.drawText(15, 220, QString("pYFeet: %1").arg(pYFeet));
//    painter.drawText(15, 200, QString("Enemy Viewport pos: %1, %2").arg(enemies.at(0)->GetGPixmapPtr()->pos().x()).arg(enemies.at(0)->GetGPixmapPtr()->pos().y()));
}

void LevelBase::UpdateLevel(Player* p, GameView *view)
{
    updateStatus = !updateStatus;

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
        {
            enemies.at(i)->UpdateEnemy();

//            if(enemies.at(i)->GetBoundingBox()->intersects(p->GetViewBB(true)->rect().toRect()))
//            {
//                emit EndOfGame(false);
//                return;
//            }
        }
    }


    //========================
    // Player Physics
    //========================

    collidedItems = p->GetViewPixmap()->collidingItems().size();
    if(collidedItems > 1)
    {
        QGraphicsItem* firstNonPlayer;

        foreach(QGraphicsItem* item, p->GetViewPixmap()->collidingItems())
        {
            if(item != p->GetViewBB(true) && item != p->GetViewPixmap())
            {
                // Check if the item is an instance of QGraphicsPixmapItem
                // Note: This is mostly useful for when we are debugging with
                //      obstacle bounding boxes visible
                if(dynamic_cast<const QGraphicsPixmapItem*>(item) != nullptr)
                {
                    firstNonPlayer = item;
                    break;
                }
            }
        }

        int idx = obstacleItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
        if(idx != -1)
        {
            qDebug() << "Obstacle idx: " << idx;
            BlockBase *nearestObsY = obstacles.at(idx);

            if(nearestObsY->GetBoundingBox()->intersects(p->GetViewBB(false)->rect().toRect()))
            {
                qDebug() << "Collision with " << idx;
                bool leftWallCollision = (((nearestObsY->GetType() == WALL_CORNER_LEFT) || ( nearestObsY->GetType() == WALL_SIDE_LEFT))
                                          && (nearestObsY->GetLeftBoundingBox()->intersects(p->GetViewBB(true)->rect().toRect())));

                bool rightWallCollision =(((nearestObsY->GetType() == WALL_CORNER_RIGHT) || (nearestObsY->GetType() == WALL_SIDE_RIGHT))
                                          && (nearestObsY->GetRightBoundingBox()->intersects(p->GetViewBB(true)->rect().toRect())));

                bool topBlockCollision = ((nearestObsY->GetTopBoundingBox()->intersects(p->GetViewBB(true)->rect().toRect())) && p->getState() == FALLING);

                if((leftWallCollision || rightWallCollision) && !topBlockCollision)
                {
                    qDebug() << "Wall collision";
                    p->setWallCollided(p->getPlayerDirection(), true);
                }
                else if(nearestObsY->GetType() == GOAL || nearestObsY->GetType() == GOAL_BASE || nearestObsY->GetType() == GOAL_MIDDLE)
                {
                    if(!leftWallCollision && !rightWallCollision)
                        p->clearWallCollided();

                    emit EndOfGame(true);
                }
                else if(nearestObsY->GetType() == NO_BLOCK_TYPE)
                {
                    qDebug() << "Empty Object collision";
                    p->setOnGround(false);
                    p->setInGap(true);
                    p->playerAction(DOWN);

                }
                else if(topBlockCollision)
                {
                    p->SetOnObstactle(true);
                    p->setPosY(obstacles.at(idx)->GetPosY() - p->getSize().y);

                    if(!leftWallCollision && !rightWallCollision)
                        p->clearWallCollided();
                }
                else if(!leftWallCollision && !rightWallCollision)
                        p->clearWallCollided();
            }
        }
//        else
//        {
//            // Enemy collision check
//            idx = enemyItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
//            if(idx != -1)
//            {
//                qDebug() << "Enemy idx: " << idx;
//                EnemyBase *nearestEnemy = enemies.at(idx);
//                if((nearestEnemy != NULL) && (nearestEnemy->GetBoundingBox()->intersects(p->GetViewBB(false)->rect().toRect())))
//                {
//                    qDebug() << "Collision with: " << idx;
//                    if((p->getState() == FALLING) || (p->getState() == SLIDING))
//                    {
//                        emit EnemyDefeated(nearestEnemy->GetValue());
//                        delete nearestEnemy;
//                    }
//                    else
//                    {
//                        emit EndOfGame(false);
//                        return;
//                    }
//                }
//            }
//        }
    }
    else
        p->clearWallCollided();
}

int LevelBase::GetLevelRightBound()
{
    return levelFloor.last()->GetRightEdge();
}

int LevelBase::getGround()
{
    return floorHeight;
}

QPoint LevelBase::GetPlayerStart()
{
    return playerStart;
}

void LevelBase::ClearView(GameView *view)
{
    foreach(QGraphicsPixmapItem *i, floorItems)
    {
        view->removePixmap(i);
        delete i;
    }

    foreach(QGraphicsPixmapItem *i, obstacleItems)
    {
        view->removePixmap(i);
        delete i;
    }

    foreach(QGraphicsPixmapItem *i, enemyItems)
    {
        view->removePixmap(i);
        delete i;
    }

    foreach(QGraphicsRectItem *r, floorBBs)
    {
        view->removeRect(r);
        delete r;
    }

    foreach(QGraphicsRectItem *r, obstacleBBs)
    {
        view->removeRect(r);
        delete r;
    }

    foreach(QGraphicsRectItem *r, enemyBBs)
    {
        view->removeRect(r);
        delete r;
    }


}

void LevelBase::generatePiece()
{

}//Draws the floor
