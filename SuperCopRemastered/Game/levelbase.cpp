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

                        // This loop is used to fill in the scene underneath the floor.
                        for(int k = 0; k < 6; k++)
                        {
                            if((bt == BLOCK_EDGE_TOP) || (bt == FLOOR_COVERED_CORNER_LEFT) || (bt == FLOOR_COVERED_CORNER_RIGHT))
                            {
                                levelFloor.push_back(new BlockBase(lt, INTERNAL_BLOCK));
                                levelFloor.last()->SetPosition(xPos * 70, floorHeight + ((k + 1) *70));
                                floorItems.push_back(view->addPixmap(*levelFloor.last()->GetTexture()));
                                floorItems.last()->setPos(xPos * 70, floorHeight + ((k + 1) * 70));
                            }
                            else if(bt == WALL_CORNER_LEFT)
                            {
                                levelFloor.push_back(new BlockBase(lt, WALL_SIDE_LEFT));
                                levelFloor.last()->SetPosition(xPos * 70, floorHeight + ((k + 1) *70));
                                floorItems.push_back(view->addPixmap(*levelFloor.last()->GetTexture()));
                                floorItems.last()->setPos(xPos * 70, floorHeight + ((k + 1) * 70));
                            }
                            else if(bt == WALL_CORNER_RIGHT)
                            {
                                levelFloor.push_back(new BlockBase(lt, WALL_SIDE_RIGHT));
                                levelFloor.last()->SetPosition(xPos * 70, floorHeight + ((k + 1) *70));
                                floorItems.push_back(view->addPixmap(*levelFloor.last()->GetTexture()));
                                floorItems.last()->setPos(xPos * 70, floorHeight + ((k + 1) * 70));
                            }

                        }
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
                                view->addRect(*bb->GetTopBoundingBox(), QPen(Qt::transparent));
                                view->addRect(*bb->GetLeftBoundingBox(), QPen(Qt::transparent));
                                view->addRect(*bb->GetRightBoundingBox(), QPen(Qt::transparent));
                                view->addRect(*bb->GetBottomBoundingBox(), QPen(Qt::transparent));
                            }
//                            obstacleBBs.push_back(view->addRect(*(bb->GetBoundingBox())));
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

                        if(eb->GetBoundingBox() != NULL)
                        {
                            enemyBBs.push_back(view->addRect(*eb->GetBoundingBox(), QPen(Qt::transparent)));
                            enemyBBs.last()->setPos(0,0);
                            enemyBBs.last()->setScale(0.5);

                            eb->SetGRectPtr(enemyBBs.last());
                        }

                        eb->SetGPixmapPtr(enemyItems.last());
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
}

void LevelBase::drawLevelBase(QPainter &painter)
{
    painter.drawText(15, 180, QString("Player items collided: %1").arg(collidedItems));
    painter.drawText(15, 190, QString("Player feet collided: %1").arg(feetItems));
    painter.drawText(15, 200, QString("Level Update: %1").arg(updateStatus));
}

void LevelBase::UpdateLevel(Player* p, GameView *view)
{
    updateStatus = !updateStatus;

    //========================
    // ENEMY UPDATES
    //========================
    for(int i = 0; i < enemies.size(); i++)
    {
        if(enemies.at(i) != NULL)
        {
            enemies.at(i)->UpdateEnemy();
        }
    }


    //========================
    // Player Physics
    //========================

    collidedItems = p->GetViewBB()->collidingItems().size();
    feetItems = p->GetFallViewBB()->collidingItems().size();

    if(p->GetPosY() > (getGround() + (5*70)))
    {
        emit EndOfGame(false);
        return;
    }

    if(collidedItems > 3)
    {
        QGraphicsItem* firstNonPlayer;

        foreach(QGraphicsItem* item, p->GetViewBB()->collidingItems())
        {
            if((item != p->GetViewBB()) && (item != p->GetViewPixmap()) && (item != p->GetFallViewBB()))
            {
                // Check if the item is an instance of QGraphicsPixmapItem
                // Note: This is makes sure we grab the pixmap and not one of the bounding boxes.
                //       We check the bounding boxes during the collision checks, but grab the index
                //       from the QGraphicsPixmapItem* vectors.
                if(dynamic_cast<const QGraphicsPixmapItem*>(item) != nullptr)
                {
                    firstNonPlayer = item;
                    break;
                }
            }
        }

        // Enemy collision check
        int idx = enemyItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);

        if(idx != -1)
        {
            qDebug() << "Enemy idx: " << idx;
            EnemyBase *nearestEnemy = enemies.at(idx);
            if((nearestEnemy != NULL) && (nearestEnemy->GetGRectPtr()->rect().toRect().intersects(p->GetFallViewBB()->rect().toRect())))
            {
                qDebug() << "Collision with enemy: " << idx  << " | state: " << p->getState() << " lastState: " << p->GetLastState();
                if(p->isJumping() && (p->getState() == FALLING || p->GetLastState() == FALLING))
                {
                    qDebug() << "Enemy Defeated";
                    emit EnemyDefeated(nearestEnemy->GetValue());
                    view->removeRect(nearestEnemy->GetGRectPtr());
                    view->removePixmap(nearestEnemy->GetGPixmapPtr());
                    delete nearestEnemy;
                    enemies.replace(idx, NULL);
                }
                else
                {
                    qDebug() << "Player Defeated";
                    emit EndOfGame(false);
                    return;
                }
            }
        }
        else
        {
            idx = obstacleItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);

            if(idx != -1)
            {
//                qDebug() << "Obstacle idx: " << idx;
                BlockBase *nearestObsY = obstacles.at(idx);

                if(nearestObsY->GetBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))
                {
//                    qDebug() << "Collision with " << idx;
                    bool leftWallCollision = (((nearestObsY->GetType() == WALL_CORNER_LEFT) || ( nearestObsY->GetType() == WALL_SIDE_LEFT) || (nearestObsY->GetType() == WALL_SIDE_RIGHT) || (nearestObsY->GetType() == WALL_CORNER_RIGHT))
                                              && (nearestObsY->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool rightWallCollision =(((nearestObsY->GetType() == WALL_CORNER_RIGHT) || (nearestObsY->GetType() == WALL_SIDE_RIGHT) || (nearestObsY->GetType() == WALL_CORNER_LEFT) || ( nearestObsY->GetType() == WALL_SIDE_LEFT))
                                              && (nearestObsY->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool topBlockCollision = ((nearestObsY->GetType() == WALL_CORNER_LEFT || nearestObsY->GetType() == WALL_CORNER_RIGHT || nearestObsY->GetType() == BLOCK || nearestObsY->GetType() == BONUS || nearestObsY->GetType() == PLATFORM_LEFT || nearestObsY->GetType() == PLATFORM_RIGHT || nearestObsY->GetType() == BLOCK_EDGE_TOP)
                                              && (nearestObsY->GetTopBoundingBox()->intersects(p->GetViewBB()->rect().toRect())) && p->getState() == FALLING);

//                    qDebug() << "Left collision: " << leftWallCollision << " Right wall collision: " << rightWallCollision << " Top block collision: " << topBlockCollision << " Block type: " << nearestObsY->GetType();

                    if((leftWallCollision || rightWallCollision) && !topBlockCollision)
                    {
//                        qDebug() << "Wall collision";
                        p->setWallCollided(p->getPlayerDirection(), true);
                        if(leftWallCollision)
                        {
                            p->setPosX(nearestObsY->GetPosX() - 50);
                        }
                        else if(rightWallCollision)
                        {
                            p->setPosX(nearestObsY->GetRightEdge());
                        }
                    }
                    else if((nearestObsY->GetType() == GOAL) || (nearestObsY->GetType() == GOAL_BASE) || (nearestObsY->GetType() == GOAL_MIDDLE))
                    {
                        if(!leftWallCollision && !rightWallCollision)
                            p->clearWallCollided();

                        emit EndOfGame(true);
                    }
                    else if(nearestObsY->GetType() == NO_BLOCK_TYPE)
                    {
//                        qDebug() << "Empty Object collision";
                        p->setOnGround(false);
                        p->setInGap(true);
                    }
                    else if(topBlockCollision)
                    {
                        p->SetOnObstactle(true);
                        p->setPosY(obstacles.at(idx)->GetPosY() - p->getSize().y);
                        p->clearWallCollided();
                    }
                    else if(!leftWallCollision && !rightWallCollision)
                            p->clearWallCollided();
                }
            }
            else
            {
                idx = floorItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
                if(idx != -1)
                {
                    BlockBase *floorBlock = levelFloor.at(idx);

                    if(floorBlock->GetBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))
                    {
                        bool leftWallCollision = (((floorBlock->GetType() == WALL_CORNER_LEFT) || ( floorBlock->GetType() == WALL_SIDE_LEFT) || (floorBlock->GetType() == WALL_SIDE_RIGHT) || (floorBlock->GetType() == WALL_CORNER_RIGHT))
                                                  && (floorBlock->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                        bool rightWallCollision =(((floorBlock->GetType() == WALL_CORNER_RIGHT) || (floorBlock->GetType() == WALL_SIDE_RIGHT) || (floorBlock->GetType() == WALL_CORNER_LEFT) || ( floorBlock->GetType() == WALL_SIDE_LEFT))
                                                  && (floorBlock->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                        bool topBlockCollision = ((floorBlock->GetType() == WALL_CORNER_LEFT || floorBlock->GetType() == WALL_CORNER_RIGHT || floorBlock->GetType() == BLOCK || floorBlock->GetType() == BONUS || floorBlock->GetType() == PLATFORM_LEFT || floorBlock->GetType() == PLATFORM_RIGHT || floorBlock->GetType() == BLOCK_EDGE_TOP)
                                                  && (floorBlock->GetTopBoundingBox()->intersects(p->GetViewBB()->rect().toRect())) && p->getState() == FALLING);

                        if((leftWallCollision || rightWallCollision) && !topBlockCollision)
                        {
    //                        qDebug() << "Wall collision";
                            p->setWallCollided(p->getPlayerDirection(), true);
                            if(leftWallCollision)
                            {
                                p->setPosX(floorBlock->GetPosX() - 50);
                            }
                            else if(rightWallCollision)
                            {
                                p->setPosX(floorBlock->GetRightEdge());
                            }
                        }
                        else
                        {
                            p->setOnGround(true);
                        }
                    }
                }
            }
        }
    }
    else
    {
        // This controls the player falling when they were previously on an obstacle
        if((feetItems < 3) && (p->getState() != JUMPING || p->getState() != FALLING) && (!p->isJumping()))
        {
//            qDebug() << "Player not on object";
            p->SetOnObstactle(false);
            p->clearWallCollided();
        }
    }
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
