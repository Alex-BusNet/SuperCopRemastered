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
    windowWidth = width;
    updateStatus = false;
    qDebug() << "Ground: " << floorHeight;
}

LevelBase::~LevelBase()
{

}

void LevelBase::LoadLevel(int levelNum, GameView *view, bool devMode)
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
                            if((bt == BLOCK_EDGE_TOP) || (bt == FLOOR_COVERED_CORNER_LEFT) || (bt == FLOOR_COVERED_CORNER_RIGHT) || (bt == INTERNAL_BLOCK))
                            {
                                levelFloor.push_back(new BlockBase(lt, INTERNAL_BLOCK));
                                levelFloor.last()->SetPosition(xPos * 70, floorHeight + ((k + 1) *70));
                                floorItems.push_back(view->addPixmap(*levelFloor.last()->GetTexture()));
                                floorItems.last()->setPos(xPos * 70, floorHeight + ((k + 1) * 70));
                            }
                            else if((bt == WALL_CORNER_LEFT) || (bt == WALL_SIDE_LEFT))
                            {
                                levelFloor.push_back(new BlockBase(lt, WALL_SIDE_LEFT));
                                levelFloor.last()->SetPosition(xPos * 70, floorHeight + ((k + 1) *70));
                                floorItems.push_back(view->addPixmap(*levelFloor.last()->GetTexture()));
                                floorItems.last()->setPos(xPos * 70, floorHeight + ((k + 1) * 70));
                            }
                            else if((bt == WALL_CORNER_RIGHT) || (bt == WALL_SIDE_RIGHT))
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
                    if(devMode)
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

                        if(bt != BONUS)
                            obstacles.push_back(new BlockBase(lt, bt));
                        else
                            obstacles.push_back(new BonusBlock(1));

                        // Obstacle is the player start location
                        if(it == 3)
                        {
                            playerStart.setX(xPos * 70);
                            playerStart.setY(floorHeight - (yPos * 90));

                            if(devMode)
                                qDebug() << "Player start: " << playerStart;
                        }
                        else
                        {
                            obstacles.last()->SetPosition(xPos * 70, floorHeight - (yPos * 70));
                        }
                    }

                    if(devMode)
                        qDebug() << "Adding Obstacles to view...";

                    // Add the obstacles to the GraphicsView
                    foreach(BlockBase *bb, obstacles)
                    {
                        if(bb != NULL)
                        {
                            obstacleItems.push_back(view->addPixmap(*(bb->GetTexture())));
                            obstacleItems.last()->setPos(bb->GetPosX(), bb->GetPosY());

                            if(devMode)
                                view->addText(QString("%1").arg(obstacleItems.size() - 1))->setPos(obstacleItems.last()->x() + 28, obstacleItems.last()->y() + 28);

                            if(bb->GetBoundingBox() != NULL)
                            {
                                QPen pen;

                                if(devMode)
                                    pen = QPen(Qt::black);
                                else
                                    pen = QPen(Qt::transparent);

                                view->addRect(*bb->GetTopBoundingBox(), pen);
                                view->addRect(*bb->GetLeftBoundingBox(), pen);
                                view->addRect(*bb->GetRightBoundingBox(), pen);
                                view->addRect(*bb->GetBottomBoundingBox(), pen);
                            }
                        }
                    }
                }
                else if(type == 3)
                {
                    // Enemy definition
                    if(devMode)
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

void LevelBase::drawLevelBase(QPainter &painter, bool devMode)
{
    if(devMode)
    {
        painter.drawText(15, 180, QString("Player items collided: %1").arg(collidedItems));
        painter.drawText(15, 190, QString("Player feet collided: %1").arg(feetItems));
        painter.drawText(15, 200, QString("Level Update: %1").arg(updateStatus));

        if(enemies.at(0) != NULL)
            enemies.at(0)->DrawEnemy(painter);
    }
}

void LevelBase::UpdateLevel(Player* p, GameView *view, bool devMode)
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
                //       We check the bounding boxes during the collision checks, but since every obstacle
                //       and the player have multiple bounding boxes, the index returned for the BB most
                //       likely won't line up with the desired object's index in the other vectors.
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
            if(devMode)
                qDebug() << "Enemy idx: " << idx;

            EnemyBase *nearestEnemy = enemies.at(idx);
            if((nearestEnemy != NULL) && (nearestEnemy->GetGRectPtr()->rect().toRect().intersects(p->GetFallViewBB()->rect().toRect())))
            {
                if(devMode)
                    qDebug() << "Collision with enemy: " << idx  << " | state: " << p->getState() << " lastState: " << p->GetLastState();

                if((p->getState() == FALLING) || (p->GetLastState() == FALLING) || (p->GetLastState() == JUMPING))
                {
                    if(devMode)
                        qDebug() << "Enemy Defeated";

                    emit EnemyDefeated(nearestEnemy->GetValue());
                    view->removeRect(nearestEnemy->GetGRectPtr());
                    view->removePixmap(nearestEnemy->GetGPixmapPtr());
                    delete nearestEnemy;
                    enemies.replace(idx, NULL);
                }
                else
                {
                    if(devMode)
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
                if(devMode)
                    qDebug() << "Obstacle idx: " << idx;

                BlockBase *nearestObsY = obstacles.at(idx);

                if(nearestObsY->GetBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))
                {
                    if(devMode)
                        qDebug() << "Collision with " << idx;

                    bool leftWallCollision = (((nearestObsY->GetType() == WALL_CORNER_LEFT) || ( nearestObsY->GetType() == WALL_SIDE_LEFT) || (nearestObsY->GetType() == WALL_SIDE_RIGHT) || (nearestObsY->GetType() == WALL_CORNER_RIGHT))
                                              && (nearestObsY->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool rightWallCollision =(((nearestObsY->GetType() == WALL_CORNER_RIGHT) || (nearestObsY->GetType() == WALL_SIDE_RIGHT) || (nearestObsY->GetType() == WALL_CORNER_LEFT) || ( nearestObsY->GetType() == WALL_SIDE_LEFT))
                                              && (nearestObsY->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool topBlockCollision = ((nearestObsY->GetType() == WALL_CORNER_LEFT || nearestObsY->GetType() == WALL_CORNER_RIGHT || nearestObsY->GetType() == BLOCK || nearestObsY->GetType() == BONUS || nearestObsY->GetType() == PLATFORM_LEFT || nearestObsY->GetType() == PLATFORM_RIGHT || nearestObsY->GetType() == BLOCK_EDGE_TOP)
                                              && (nearestObsY->GetTopBoundingBox()->intersects(p->GetViewBB()->rect().toRect())) && p->getState() == FALLING);

                    bool bottomBlockCollision = ((nearestObsY->GetType() == BONUS) && (nearestObsY->GetBottomBoundingBox()->intersects(p->GetJumpViewBB()->rect().toRect())));

                    if(devMode)
                        qDebug() << "Left collision: " << leftWallCollision << " Right wall collision: " << rightWallCollision << " Top block collision: " << topBlockCollision << " Bottom block collision: " << bottomBlockCollision << " Block type: " << nearestObsY->GetType();

                    if((leftWallCollision || rightWallCollision) && !topBlockCollision && !bottomBlockCollision)
                    {
                        if(devMode)
                            qDebug() << "Wall collision";

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
                        if(p->getState() != VICTORY)
                        {
                            if(!leftWallCollision && !rightWallCollision)
                                p->clearWallCollided();

                            emit EndOfGame(true);
                        }
                    }
                    else if(((!leftWallCollision && !rightWallCollision && !topBlockCollision && !bottomBlockCollision) && (nearestObsY->GetType() == GAP_BLOCK)))
                    {
                        if(devMode)
                            qDebug() << "Empty Object collision";

                        p->setOnGround(false);
                        p->setInGap(true);
                    }
                    else if(bottomBlockCollision)
                    {
                        if(devMode)
                            qDebug() << "Bonus block collision";

                        donuts.push_back(new ItemBase(DONUT));
                        donuts.last()->SetPostion(nearestObsY->GetPosX(), nearestObsY->GetPosY() - 64);

                        donutItems.push_back(view->addPixmap(*donuts.last()->GetTexture()));
                        donutItems.last()->setPos(nearestObsY->GetPosX(), nearestObsY->GetPosY() - 64);
                        donuts.last()->SetViewPixmap(donutItems.last());

                        donutBBs.push_back(view->addRect(*donuts.last()->GetBoundingBox()));
                        donuts.last()->SetViewBB(donutBBs.last());

                        ((BonusBlock*)obstacles.at(idx))->BlockHit();
                        if(((BonusBlock*)obstacles.at(idx))->GetHitsRemaining() <= 0)
                        {
                            obstacles.at(idx)->SetType(NO_LEVEL_TYPE, NO_BLOCK_TYPE);
                            obstacleItems.at(idx)->setPixmap(*obstacles.at(idx)->GetTexture());
                        }

                        p->playerAction(NONE);
                    }
                    else if(topBlockCollision)
                    {
                        p->SetOnObstactle(true);
                        p->setPosY(obstacles.at(idx)->GetPosY() - 89);
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

                    // This block is nearly identical to the obstacle collision check
                    // since we need to find the walls of gaps when the player is falling.
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
                            if(devMode)
                                qDebug() << "Gap Wall collision";

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
                            // If the player didn't collide with the left or right side of
                            // a floor object, then they must be standing on the floor.
                            p->setOnGround(true);
                        }
                    }
                }
                else
                {
                    // Donut Collision check
                    idx = donutItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
                    if(idx != -1)
                    {
                        if(devMode)
                            qDebug() << "Donut collision";

                        emit EnemyDefeated(10);
                        view->removePixmap(donuts.at(idx)->GetViewPixmap());
                        view->removeRect(donuts.at(idx)->GetViewBB());
                        delete donuts.at(idx);
                        donuts.replace(idx, NULL);
                    }
                    else
                    {
                        // Player just falls.
                        if(p->getState() != JUMPING && p->getState() != FALLING)
                        {
                            p->setOnGround(false);
                            p->SetOnObstactle(false);
                        }
                    }
                }
            }
        }
    }
    else
    {
        // This controls the player falling when they were previously on an obstacle
        // We need to check if the player isn't attempting to jump here otherwise
        // this block of code will pull the player down as soon as they try to jump.
        if((feetItems < 3) && (p->getState() != JUMPING || p->getState() != FALLING) && (!p->isJumping()))
        {
            if(devMode)
                qDebug() << "Player not on object";

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
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }

    foreach(QGraphicsPixmapItem *i, obstacleItems)
    {
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }

    foreach(QGraphicsPixmapItem *i, enemyItems)
    {
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }

    foreach(QGraphicsRectItem *r, floorBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }

    foreach(QGraphicsRectItem *r, obstacleBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }

    foreach(QGraphicsRectItem *r, enemyBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }
}

void LevelBase::generatePiece()
{

}//Draws the floor
