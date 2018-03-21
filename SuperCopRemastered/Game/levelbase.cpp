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
    nullHolder = NULL;
    qDebug() << "Ground: " << floorHeight;
}

LevelBase::~LevelBase()
{

}

void LevelBase::LoadLevel(int levelNum, GameView *view, bool devMode)
{
    nullHolder = view->addRect(*(new QRect(0,0,5,5)));
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

                        if(devMode)
                            view->addText(QString("%1").arg(floorItems.size() / 7))->setPos((xPos * 70) + 30, floorHeight - (yPos * 70) + 30);

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

                        if(bt != BONUS && it != 3)
                            obstacles.push_back(new BlockBase(lt, bt));
                        else if(it != 3)
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
                        enemies.last()->SetBounds(lbound * 70, (rbound+1) * 70);
                    }

                    foreach(EnemyBase *eb, enemies)
                    {
                        if(eb != NULL)
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

                    if(devMode)
                        qDebug() << "Level load complete.";
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

//    if(devMode)
//    {
//        // Draw Vertical lines
//        for(int i = 0; i < levelFloor.size() / 7; i++)
//        {
//            view->addLine((i * 70), 0, (i * 70), view->height());
//        }

//        // Draw Horizontal lines
//        for(int i = 0; i < 16; i++)
//        {
//            view->addLine(0, (i * 70) - 20, ((levelFloor.size() / 7) * 70), (i * 70));
//            view->addText(QString("%1").arg(i))->setPos(15, (i * 70) + 15);
//        }
//    }

    nullHolder->ensureVisible(0,0,5,5,1,1);

}

void LevelBase::drawLevelBase(QPainter &painter, bool devMode)
{
    if(devMode)
    {
        painter.drawText(20, 190, QString("Player items collided: %1").arg(collidedItems));
        painter.drawText(20, 200, QString("Player feet collided: %1").arg(feetItems));
        painter.drawText(20, 210, QString("Level Update: %1").arg(updateStatus));

        if(enemies.at(0) != NULL)
            enemies.at(0)->DrawEnemy(painter);
    }

    // Renders the parsed view array on screen
    int x = 0, y = 0;
    for(y = 0; y < 10; y++)
    {
        for(x = 0; x < 18; x++)
        {
            if(parsedView[y][x] != 0)
                painter.drawText((15 * x) + 215, (15 * y) + 50, QString(" %1 ").arg(parsedView[y][x]));
        }
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
            if((enemyItems.at(i) != NULL) && (enemyItems.at(i)->collidingItems().size() > 1))
            {
                QGraphicsItem *item = enemyItems.at(i)->collidingItems().at(1);
                int idx = enemyItems.indexOf((QGraphicsPixmapItem*)item);
                // We just want the enemies to turn around when they collide with each other.
                // We will handle collisions with player later.
                if(idx != -1)
                {
                    enemies.at(i)->FlipDirection();
                    enemies.at(idx)->FlipDirection();
                }
                else
                {
                    idx = obstacleItems.indexOf((QGraphicsPixmapItem*)item);
                    if(idx != -1)
                        enemies.at(i)->FlipDirection();
                    else
                        enemies.at(i)->UpdateEnemy();
                }
            }
            else
                enemies.at(i)->UpdateEnemy();
        }
    }

    //========================
    // Player Physics
    //========================

    if(p->getState() == FALLING)
        collidedItems = p->GetFallViewBB()->collidingItems().size();
    else if(p->getState() == JUMPING)
        collidedItems = p->GetJumpViewBB()->collidingItems().size();
    else
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
        QList<QGraphicsItem*> collideList;

        if(p->getState() == FALLING)
            collideList = p->GetFallViewBB()->collidingItems();
        else if(p->getState() == JUMPING)
            collideList = p->GetJumpViewBB()->collidingItems();
        else
            collideList = p->GetViewBB()->collidingItems();

        foreach(QGraphicsItem* item, collideList)
        {
            if((item != p->GetViewBB()) && (item != p->GetViewPixmap()))
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

                if((p->getState() == FALLING) || (p->GetLastState() == JUMPING) || (p->GetLastState() == FALLING))
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

                    BlockType bt = nearestObsY->GetType();
                    bool leftWallCollision = (((bt == WALL_CORNER_LEFT) || (bt == WALL_SIDE_LEFT) || (bt == WALL_SIDE_RIGHT) || (bt == WALL_CORNER_RIGHT)
                                               ||(bt == PLATFORM_LEFT) || (bt == PLATFORM_RIGHT) || (bt == BONUS) || (bt == BLOCK_EDGE_TOP) || (bt == BLOCK))
                                              && (nearestObsY->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool rightWallCollision =(((bt == WALL_CORNER_RIGHT) || (bt == WALL_SIDE_RIGHT) || (bt == WALL_CORNER_LEFT) || (bt == WALL_SIDE_LEFT)
                                               || (bt == PLATFORM_LEFT) || (bt == PLATFORM_RIGHT) || (bt == BONUS) || (bt == BLOCK_EDGE_TOP) || (bt == BLOCK))
                                              && (nearestObsY->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool topBlockCollision = (((bt == WALL_CORNER_LEFT) || (bt == WALL_CORNER_RIGHT) || (bt == BLOCK) || (bt == BONUS)
                                               ||(bt == PLATFORM_LEFT) || (bt == PLATFORM_RIGHT) || (bt == BLOCK_EDGE_TOP))
                                              && (nearestObsY->GetTopBoundingBox()->intersects(p->GetFallViewBB()->rect().toRect()) /*|| (nearestObsY->GetTopBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))*/)
                                              && ((p->getState() == FALLING) || (p->getState() == IDLE)));

                    bool bottomBlockCollision = ((bt == BONUS) && (nearestObsY->GetBottomBoundingBox()->intersects(p->GetJumpViewBB()->rect().toRect())) && p->getState() == JUMPING);

                    if(devMode)
                    {
                        qDebug() << "Left collision: " << leftWallCollision << " Right wall collision: " << rightWallCollision << " Top block collision: " << topBlockCollision << " Bottom block collision: " << bottomBlockCollision << " Block type: " << nearestObsY->GetType();
                    }

                    if(leftWallCollision && !topBlockCollision)
                    {
                        if(devMode)
                            qDebug() << "Left Wall collision";

                        p->setWallCollided(EAST, true);
                        p->setPosX(nearestObsY->GetPosX() - 50);

                    }
                    else if(rightWallCollision && !topBlockCollision)
                    {
                        if(devMode)
                            qDebug() << "Right Wall Collision";

                        p->setWallCollided(WEST, true);
                        p->setPosX(nearestObsY->GetRightEdge());
                    }
                    else if(topBlockCollision)
                    {
                        if(devMode)
                            qDebug() << "Player on obstacle";

                        if(!p->isOnObstacle())
                        {
                            p->SetOnObstactle(true, nearestObsY->GetPosY());
                            p->setPosY(obstacles.at(idx)->GetPosY() - 89);
                            p->clearWallCollided();
                        }
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
                    else if((bt == GOAL) || (bt == GOAL_BASE) || (bt == GOAL_MIDDLE))
                    {
                        if(p->getState() != VICTORY)
                        {
                            if(devMode)
                                qDebug() << "Goal collision";

                            if(!leftWallCollision && !rightWallCollision)
                                p->clearWallCollided();

                            emit EndOfGame(true);
                        }
                    }
                    else if(((!leftWallCollision && !rightWallCollision && !topBlockCollision && !bottomBlockCollision) && (bt == GAP_BLOCK)))
                    {
                        if(devMode)
                            qDebug() << "Empty Object collision";

                        p->SetOnObstactle(false, 0);
                    }
                    else if(!leftWallCollision && !rightWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Clearing wall collisions";

                        p->clearWallCollided();
                    }
                }
            }
//            else
//            {
                idx = floorItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
                if(idx != -1)
                {
                    BlockBase *floorBlock = levelFloor.at(idx);

                    // This block is nearly identical to the obstacle collision check
                    // since we need to find the walls of gaps when the player is falling.
                    if(floorBlock->GetBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))
                    {
                        BlockType bt = floorBlock->GetType();
                        bool leftWallCollision = (((bt == WALL_CORNER_LEFT) || (bt == WALL_SIDE_LEFT)
                                                  || (bt == BLOCK_EDGE_TOP) || (bt == BLOCK))
                                                  && (floorBlock->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                        bool rightWallCollision =(((bt == WALL_CORNER_RIGHT) || (bt == WALL_SIDE_RIGHT) /*|| (bt == WALL_CORNER_LEFT) || (bt == WALL_SIDE_LEFT)*/
                                                   /*|| (bt == PLATFORM_LEFT) || (bt == PLATFORM_RIGHT) || (bt == BONUS)*/ || (bt == BLOCK_EDGE_TOP) || (bt == BLOCK))
                                                  && (floorBlock->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                        bool topBlockCollision = (((bt == WALL_CORNER_LEFT) || (bt == WALL_CORNER_RIGHT) || (bt == BLOCK) || (bt == BONUS)
                                                   ||(bt == PLATFORM_LEFT) || (bt == PLATFORM_RIGHT) || (bt == BLOCK_EDGE_TOP))
                                                  && (floorBlock->GetTopBoundingBox()->intersects(p->GetFallViewBB()->rect().toRect()) /*|| (nearestObsY->GetTopBoundingBox()->intersects(p->GetViewBB()->rect().toRect()))*/)
                                                  && ((p->getState() == FALLING) || (p->getState() == IDLE)));

                        if(leftWallCollision && !topBlockCollision)
                        {
                            if(devMode)
                                qDebug() << "Left Gap Wall collision";

                            p->setWallCollided(EAST, true);
                            p->setPosX(floorBlock->GetPosX() - 50);

                        }
                        else if(rightWallCollision && !topBlockCollision)
                        {
                            if(devMode)
                                qDebug() << "Right Gap Wall collision";
                            p->setWallCollided(WEST, true);
                            p->setPosX(floorBlock->GetRightEdge());
                        }
                        else
                        {
                            // If the player didn't collide with the left or right side of
                            // a floor object, and the floor object is not an opening, then
                            // the player must be standing on the ground.
                            if(floorBlock->GetType() != NO_BLOCK_TYPE && floorBlock->GetType() != GAP_BLOCK)
                                p->SetOnObstactle(true, floorBlock->GetPosY());
                        }
                    }
                }
//                else
//                {
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
//                    else
//                    {
                        // Player just falls.
//                        if(p->getState() != JUMPING && p->getState() != FALLING)
//                        {
//                            if(devMode)
//                                qDebug() << "Player falling";

//                            p->setOnGround(false);
//                            p->SetOnObstactle(false);
//                        }
//                    }
//                }
//            }
        }
    }
    else
    {
        // This controls the player falling when they were previously on an obstacle
        // We need to check if the player isn't attempting to jump here otherwise
        // this block of code will pull the player down as soon as they try to jump.
        if((feetItems < 3) && (p->getState() != JUMPING && p->getState() != FALLING) && (!p->isJumping()))
        {
            if(devMode)
                qDebug() << "Player not on object";

            p->SetOnObstactle(false, 0);
            p->clearWallCollided();
        }
        else
            if((collidedItems < 3) && (p->getState() == JUMPING))
                p->clearWallCollided();
    }

    // ===============================
    // Neural Net / Genetic Algorithm
    // Screen Parsing
    //================================
    int x, y;

    // Clear the parsedView array
    for(y = 0; y < 10; y++)
    {
        for(x = 0; x < 18; x++)
        {
            parsedView[y][x] = 0;
        }
    }

    x = y = 0;

    // Set the position of any obstacles visible to the player
    foreach(QGraphicsPixmapItem* item, obstacleItems)
    {
        int idx = obstacleItems.indexOf(item);
        if(idx != -1)
        {
            QPoint pos = view->mapFromScene(obstacles.at(idx)->GetPosX(), obstacles.at(idx)->GetPosY());
            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
            {
                x = (pos.x() / 70) % 18;
                y = (pos.y() / 70) % 10;

                if(obstacles.at(idx)->GetType() != NO_BLOCK_TYPE && obstacles.at(idx)->GetType() != GAP_BLOCK)
                    parsedView[y][x] = 1;
            }

            // If we are looking at items out side the right most part of the screen,
            // stop searching through vector.
            // Note: This provides some improved performance near the start of the level
            //       (Not that any one will actually notice the improvement), but is
            //       reduced near the end of the level since the time complexity of the
            //       loop becomes O(n)
            if(pos.x() >= view->width())
                break;
        }
    }

    // Set the position of floor tiles visible to the player
    foreach(QGraphicsPixmapItem* item, floorItems)
    {
        int idx = floorItems.indexOf(item);
        if(idx != -1)
        {
            QPoint pos = view->mapFromScene(levelFloor.at(idx)->GetPosX(), levelFloor.at(idx)->GetPosY());

            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
            {
                x = (pos.x() / 70) % 18;
                y = (pos.y() / 70) % 10;

                if(levelFloor.at(idx)->GetType() != NO_BLOCK_TYPE && levelFloor.at(idx)->GetType() != GAP_BLOCK)
                    parsedView[y][x] = 1;
            }

            // If we are looking at items out side the right most part of the screen,
            // stop searching through vector.
            // Note: This provides some improved performance near the start of the level
            //       (Not that any one will actually notice the improvement), but is
            //       reduced near the end of the level since the time complexity of the
            //       loop becomes O(n)
            if(pos.x() > view->width())
                break;
        }
    }

    // Set the position of any enemies visible to the player
    foreach(QGraphicsPixmapItem* item, enemyItems)
    {
        if(enemies.at(enemyItems.indexOf(item)) != NULL)
        {
            int idx = enemyItems.indexOf(item);
            if(idx != -1)
            {
                QPoint pos = view->mapFromScene(enemies.at(idx)->GetPosX(), enemies.at(idx)->GetPosY());

                if(pos.x() >= 0 && pos.x() <= view->width() && pos.y() >= 0 && pos.y() <= view->height())
                {
                    x = (pos.x() / 70) % 18;
                    y = (pos.y() / 70) % 10;

                    parsedView[y][x] = 3;
                }

                // If we are looking at items out side the right most part of the screen,
                // stop searching through vector.
                // Note: This provides some improved performance near the start of the level
                //       (Not that any one will actually notice the improvement), but is
                //       reduced near the end of the level since the time complexity of the
                //       loop becomes O(n)
                if(pos.x() > view->width())
                    break;
            }
        }
    }

    // Set the position of any donuts visible to the player
    foreach(QGraphicsPixmapItem *item, donutItems)
    {
        if(item != NULL)
        {
            if(donuts.at(donutItems.indexOf(item)) != NULL)
            {
                int idx = donutItems.indexOf(item);
                if(idx != -1)
                {
                    QPoint pos = view->mapFromScene(donuts.at(idx)->GetPosX(), donuts.at(idx)->GetPosY());

                    if(pos.x() >= 0 && pos.x() <= view->width() && pos.y() >= 0 && pos.y() <= view->height())
                    {
                        x = (pos.x() / 70) % 18;
                        y = ((pos.y() / 70) % 10);

                        parsedView[y][x] = 4;
                    }

                    // If we are looking at items out side the right most part of the screen,
                    // stop searching through vector.
                    // Note: This provides some improved performance near the start of the level
                    //       (Not that anywould will actually notice the improvement), but is
                    //       reduced near the end of the level since the time complexity of the
                    //       loop becomes O(n)
                    if(pos.x() > view->width())
                        break;
                }
            }
        }
    }

    // Set the player's location in the array.
    if(p != NULL)
    {
        QPoint pos = view->mapFromScene(p->GetPosX(), p->GetPosY());

        if(pos.x() >= 0 && pos.x() <= view->width() && pos.y() >= 0 && pos.y() <= view->height())
        {
            x = ((int)pos.x() / 70) % 18;
            y = ((int)pos.y() / 70) % 10;

            parsedView[y][x] = 2;
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
    foreach(EnemyBase *eb, enemies)
    {
        if(eb != NULL)
            delete eb;
    }
    enemies.clear();

    foreach(BlockBase *bb, obstacles)
    {
        if(bb != NULL)
            delete bb;
    }
    obstacles.clear();

    foreach(BlockBase *bb, levelFloor)
    {
        if(bb != NULL)
            delete bb;
    }
    levelFloor.clear();

    foreach(QGraphicsPixmapItem *i, floorItems)
    {
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }
    floorItems.clear();

    foreach(QGraphicsPixmapItem *i, obstacleItems)
    {
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }
    obstacleItems.clear();

    foreach(QGraphicsPixmapItem *i, enemyItems)
    {
        if(i != NULL)
        {
            view->removePixmap(i);
            delete i;
        }
    }
    enemyItems.clear();

    foreach(QGraphicsRectItem *r, floorBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }
    floorBBs.clear();

    foreach(QGraphicsRectItem *r, obstacleBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }
    obstacleBBs.clear();

    foreach(QGraphicsRectItem *r, enemyBBs)
    {
        if(r != NULL)
        {
            view->removeRect(r);
            delete r;
        }
    }
    enemyBBs.clear();

    view->ClearScene();

}//Draws the floor
