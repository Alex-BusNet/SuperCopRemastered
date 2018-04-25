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

    int x, y;

    // Clear the parsedView array
    for(y = 0; y < 10; y++)
    {
        for(x = 0; x < 15; x++)
        {
            parsedView[y][x] = 0;
        }
    }
    qDebug() << "Ground: " << floorHeight;
}

LevelBase::~LevelBase()
{

}

void LevelBase::LoadLevel(int levelNum, GameView *view, bool devMode)
{
    nullHolder = view->addRect(*(new QRect(0,0,5,5)), QPen(Qt::transparent));
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
                        {
                            QGraphicsTextItem *i = view->addText(QString("%1").arg(floorItems.size() / 7));
                            i->setPos((xPos * 70) + 30, floorHeight - (yPos * 70) + 30);

                            if(lt == GRASS)
                                i->setDefaultTextColor(Qt::black);
                            else
                                i->setDefaultTextColor(Qt::white);
                        }

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

                    foreach(BlockBase *bb, levelFloor)
                    {
                        if(bb->GetBoundingBox() != NULL && (bb->GetType() != INTERNAL_BLOCK && bb->GetType() != FLOOR_COVERED_CORNER_LEFT && bb->GetType() != FLOOR_COVERED_CORNER_RIGHT))
                        {
                            QPen pen;

                            pen = QPen(Qt::transparent);

                            floorBBs.push_back(view->addRect(*bb->GetTopBoundingBox(), pen));
                            floorBBs.push_back(view->addRect(*bb->GetLeftBoundingBox(), pen));
                            floorBBs.push_back(view->addRect(*bb->GetRightBoundingBox(), pen));
                            floorBBs.push_back(view->addRect(*bb->GetBottomBoundingBox(), pen));
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
                            {
                                QGraphicsTextItem *i = view->addText(QString("%1").arg(obstacleItems.size() - 1));
                                i->setPos(obstacleItems.last()->x() + 28, obstacleItems.last()->y() + 28);

                                if(levelFloor.first()->GetLevelType() == GRASS)
                                    i->setDefaultTextColor(Qt::black);
                                else
                                    i->setDefaultTextColor(Qt::white);
                            }

                            if(bb->GetBoundingBox() != NULL)
                            {
                                QPen pen;

                                if(devMode)
                                {
                                    if(levelFloor.at(0)->GetLevelType() == GRASS)
                                        pen = QPen(Qt::black);
                                    else
                                        pen = QPen(Qt::white);
                                }
                                else
                                    pen = QPen(Qt::transparent);

                                obstacleBBs.push_back(view->addRect(*bb->GetBoundingBox(), pen));
                                obstacleBBs.push_back(view->addRect(*bb->GetTopBoundingBox(), pen));
                                bb->SetTopGBB(obstacleBBs.last());
                                obstacleBBs.push_back(view->addRect(*bb->GetLeftBoundingBox(), pen));
                                bb->SetLeftGBB(obstacleBBs.last());
                                obstacleBBs.push_back(view->addRect(*bb->GetRightBoundingBox(), pen));
                                bb->SetRightGBB(obstacleBBs.last());
                                obstacleBBs.push_back(view->addRect(*bb->GetBottomBoundingBox(), pen));
                                bb->SetBottomGBB(obstacleBBs.last());
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
                        enemies.last()->SetBounds(lbound * 70, (rbound+1) * 70);
                        enemies.last()->SetStartParams(xPos * 70, floorHeight - (yPos * 60), dir);
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
        // Draw Vertical lines
//        for(int i = 0; i < levelFloor.size() / 7; i++)
//        {
//            view->addLine((i * 70), 0, (i * 70), view->height());
//        }

        // Draw Horizontal lines
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
    if(levelFloor.first()->GetLevelType() == GRASS)
        painter.setPen(QPen(Qt::black));
    else
        painter.setPen(QPen(Qt::white));

    if(devMode)
    {
        painter.drawText(20, 200, QString("Player items collided: %1").arg(collidedItems));
        painter.drawText(20, 210, QString("Player feet collided: %1").arg(feetItems));
        painter.drawText(20, 220, QString("Level Update: %1").arg(updateStatus));

        if(enemies.at(0) != NULL)
            enemies.at(0)->DrawEnemy(painter);


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

        painter.drawRect(210, 45, (15*18) + 5, (10*15) + 5);
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

    if(p->getState() == JUMPING)
        collidedItems = p->GetJumpViewBB()->collidingItems().size();
    else
        collidedItems = p->GetViewBB()->collidingItems().size();

    feetItems = p->GetFallViewBB()->collidingItems().size();

    if(p->GetPosY() + (70) > (getGround() + (0)))
    {
        emit EndOfGame(false);
        return;
    }

    if(collidedItems > 3)
    {
        QGraphicsItem* firstNonPlayer;
        QList<QGraphicsItem*> collideList;
        QRect pCollisionRect;

        if(p->getState() == FALLING)
        {
            collideList = p->GetFallViewBB()->collidingItems();
            pCollisionRect = p->GetFallViewBB()->rect().toRect();
        }
        else if(p->getState() == JUMPING)
        {
            collideList = p->GetJumpViewBB()->collidingItems();
            pCollisionRect = p->GetJumpViewBB()->rect().toRect();
        }
        else
        {
            collideList = p->GetFallViewBB()->collidingItems();
            pCollisionRect = p->GetFallViewBB()->rect().toRect();
        }

        foreach(QGraphicsItem* item, collideList)
        {
            if((item != p->GetViewBB()) && (item != p->GetViewPixmap()) && (item != p->GetJumpViewBB() && (item != p->GetFallViewBB())))
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
            if((nearestEnemy != NULL) && (nearestEnemy->GetGRectPtr()->rect().toRect().intersects(pCollisionRect)))
            {
                if(devMode)
                    qDebug() << "Collision with enemy: " << idx  << " | state: " << p->getState() << " lastState: " << p->GetLastState();

                if((p->getState() == FALLING) || (p->GetLastState() == JUMPING))
                {
                    if(devMode)
                        qDebug() << "Enemy Defeated";

                    emit EnemyDefeated(nearestEnemy->GetValue());
                    view->removeRect(nearestEnemy->GetGRectPtr());
                    view->removePixmap(nearestEnemy->GetGPixmapPtr());
                    enemies.at(idx)->Toggle();

                    // Player gets a little 'bounce' when they kill enemies
                    p->setPosY(p->GetPosY() - 40);
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

        // Obstacle Collision check
        idx = obstacleItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);

        if(idx != -1)
        {
            if(devMode)
                qDebug() << "Obstacle idx: " << idx;

            BlockBase *nearestObsY = obstacles.at(idx);

            if(nearestObsY->GetBoundingBox()->intersects(pCollisionRect))
            {
                if(devMode)
                    qDebug() << "Collision with " << idx;

                BlockType bt = nearestObsY->GetType();
                if(bt == NO_BLOCK_TYPE || bt == GAP_BLOCK)
                {
                    p->SetOnObstactle(false, 0);
                }
                else
                {
                    bool leftWallCollision = (((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (nearestObsY->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool rightWallCollision =(((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (nearestObsY->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool topBlockCollision = (((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (nearestObsY->GetTopBoundingBox()->intersects(p->GetFallViewBB()->rect().toRect()))
                                              && (p->getState() != JUMPING));

                    bool bottomBlockCollision = ((bt == BONUS) && (nearestObsY->GetBottomBoundingBox()->intersects(p->GetJumpViewBB()->rect().toRect())) && p->getState() == JUMPING);

                    if(devMode)
                    {
                        qDebug() << "Left collision: " << leftWallCollision << " Right wall collision: " << rightWallCollision << " Top block collision: " << topBlockCollision << " Bottom block collision: " << bottomBlockCollision << " Block type: " << nearestObsY->GetType();
                    }

                    if(topBlockCollision && !leftWallCollision && !rightWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Player on obstacle";

                        if(!p->isOnObstacle())
                        {
                            p->SetOnObstactle(true, nearestObsY->GetPosY());
                            p->setPosY(obstacles.at(idx)->GetPosY() - 95);
                            p->clearWallCollided();
                        }
                    }

                    else if(leftWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Left Wall collision";

                        p->setWallCollided(EAST, true);
                        p->setPosX(nearestObsY->GetPosX() - 80);

                    }
                    else if(rightWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Right Wall Collision";

                        p->setWallCollided(WEST, true);
                        p->setPosX(nearestObsY->GetRightEdge() - 20);
                    }
//                    else if(bottomBlockCollision)
//                    {
//                        if(devMode)
//                            qDebug() << "Bonus block collision";

//                        donuts.push_back(new ItemBase(DONUT));
//                        donuts.last()->SetPostion(nearestObsY->GetPosX(), nearestObsY->GetPosY() - 64);

//                        donutItems.push_back(view->addPixmap(*donuts.last()->GetTexture()));
//                        donutItems.last()->setPos(nearestObsY->GetPosX(), nearestObsY->GetPosY() - 64);
//                        donuts.last()->SetViewPixmap(donutItems.last());

//                        donutBBs.push_back(view->addRect(*donuts.last()->GetBoundingBox()));
//                        donuts.last()->SetViewBB(donutBBs.last());

//                        ((BonusBlock*)obstacles.at(idx))->BlockHit();
//                        if(((BonusBlock*)obstacles.at(idx))->GetHitsRemaining() <= 0)
//                        {
//                            obstacles.at(idx)->SetType(levelFloor.at(0)->GetLevelType(), BLOCK_EDGE_TOP);
//                            obstacleItems.at(idx)->setPixmap(*obstacles.at(idx)->GetTexture());
//                        }

//                        p->playerAction(NONE, false, true);
//                    }
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
                    else if(((!leftWallCollision && !rightWallCollision && !topBlockCollision && !bottomBlockCollision) && (bt == GAP_BLOCK || bt == NO_BLOCK_TYPE)))
                    {
                        if(devMode)
                            qDebug() << "Empty Object collision";

                        p->SetOnObstactle(false, 0);
                    }
                }
            }
        }

        // Floor collision check
        idx = floorItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
        if(idx != -1)
        {
            BlockBase *floorBlock = levelFloor.at(idx);

            // This block is nearly identical to the obstacle collision check
            // since we need to find the walls of gaps when the player is falling.
            if(floorBlock->GetBoundingBox()->intersects(pCollisionRect))
            {
                BlockType bt = floorBlock->GetType();
                if(bt == NO_BLOCK_TYPE || bt == GAP_BLOCK)
                {
                    p->SetOnObstactle(false, 0);
                }
                else
                {
                    bool leftWallCollision = (((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (floorBlock->GetLeftBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool rightWallCollision =(((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (floorBlock->GetRightBoundingBox()->intersects(p->GetViewBB()->rect().toRect())));

                    bool topBlockCollision = (((bt != GOAL) && (bt != GOAL_MIDDLE))
                                              && (floorBlock->GetTopBoundingBox()->intersects(p->GetFallViewBB()->rect().toRect()))
                                              && (p->getState() != JUMPING));

//                    qDebug() << "Floor " << "leftWall: " << leftWallCollision << " rightWall: " << rightWallCollision << " topBlock: " << topBlockCollision;
                    if(topBlockCollision)
                    {
                        // If the player didn't collide with the left or right side of
                        // a floor object, and the floor object is not an opening, then
                        // the player must be standing on the ground.
//                        qDebug() << "Collision with top of floor object";
                        if(!p->isOnObstacle())
                            p->SetOnObstactle(true, floorBlock->GetPosY());
    //                    p->clearWallCollided();
                    }

                    if(leftWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Left Gap Wall collision";

                        p->setWallCollided(EAST, true);
                        p->setPosX(floorBlock->GetPosX() - 105);

                    }
                    else if(rightWallCollision)
                    {
                        if(devMode)
                            qDebug() << "Right Gap Wall collision";
                        p->setWallCollided(WEST, true);
                        p->setPosX(floorBlock->GetRightEdge() - 20);
                    }
    //                else
    //                {
    //                    if(devMode)
    //                        qDebug() << "Floor Clearing wall collisions";
    //                    p->clearWallCollided();
    //                }
                }
            }
        }

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
    }
    else
    {
        QGraphicsItem* firstNonPlayer;
        QList<QGraphicsItem*> collideList;
        QRect pCollisionRect;

        if(p->getState() == FALLING)
        {
            collideList = p->GetFallViewBB()->collidingItems();
            pCollisionRect = p->GetFallViewBB()->rect().toRect();
        }
        else if(p->getState() == JUMPING)
        {
            collideList = p->GetJumpViewBB()->collidingItems();
            pCollisionRect = p->GetJumpViewBB()->rect().toRect();
        }
        else
        {
            collideList = p->GetViewBB()->collidingItems();
            pCollisionRect = p->GetViewBB()->rect().toRect();
        }

        foreach(QGraphicsItem* item, collideList)
        {
            if((item != p->GetViewBB()) && (item != p->GetViewPixmap()) && (item != p->GetJumpViewBB() && (item != p->GetFallViewBB())))
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

        // This controls the player falling when they were previously on an obstacle
        // We need to check if the player isn't attempting to jump here otherwise
        // this block of code will pull the player down as soon as they try to jump.
        int fIdx = floorItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
        if(fIdx != -1)
        {
            if(p->getState() != JUMPING && p->getState() != FALLING && (levelFloor.at(fIdx)->GetType() == NO_BLOCK_TYPE || levelFloor.at(fIdx)->GetType() == GAP_BLOCK))
            {
                p->SetOnObstactle(false, 0);
                p->clearWallCollided();
            }
            else if((collidedItems < 3) && (p->getState() == JUMPING))
                    p->clearWallCollided();
        }

        int oIdx = obstacleItems.indexOf((QGraphicsPixmapItem*)firstNonPlayer);
        if(oIdx != -1)
        {
            if(p->getState() != JUMPING && p->getState() != FALLING && (obstacles.at(oIdx)->GetType() == NO_BLOCK_TYPE || obstacles.at(oIdx)->GetType() == GAP_BLOCK))
            {
                p->SetOnObstactle(false, 0);
                p->clearWallCollided();
            }
            else if((collidedItems < 3) && (p->getState() == JUMPING))
            {
                p->clearWallCollided();
            }
        }

        if(fIdx == -1 && oIdx == -1 && feetItems < 3 && p->getState() != JUMPING)
        {
            p->SetOnObstactle(false, 0);
//            if(devMode)
//                qDebug() << "FeetItems Clearing wall collisions";
            p->clearWallCollided();
        }
    }

    // ===============================
    // Neural Net / Genetic Algorithm
    // Screen Parsing
    //================================
    int x, y;

    // Clear the parsedView array
    for(y = 0; y < 10; y++)
    {
        for(x = 0; x < 15; x++)
        {
            parsedView[y][x] = 0;
        }
    }

    x = y = 0;
    int pBlockX = 7, pBlockY = 4;
    // Set the player's location in the array.
    if(p != NULL)
    {
        QPoint pos = view->mapFromScene(p->GetPosX(), p->GetPosY());

//        if(pos.x() >= 0 && pos.x() <= view->width() && pos.y() >= 0 && pos.y() <= view->height())
//        {
            pBlockX = (pos.x() / 70) + 1;
//            x = pBlockX % 15;
            pBlockY = (pos.y() / 70);
//            y = pBlockY % 10;

//            if(x < 0) { x = 0; }

//            if(y >= 0 && y < 10)
                parsedView[4][7] = 2;

//            if((y + 1) >= 0 && (y + 1) < 10)
                parsedView[5][7] = 2;

//        }
    }

    // Set the position of any obstacles visible to the player
    foreach(QGraphicsPixmapItem* item, obstacleItems)
    {
        int idx = obstacleItems.indexOf(item);
        if(idx != -1)
        {
            QPoint pos = view->mapFromScene(obstacles.at(idx)->GetPosX(), obstacles.at(idx)->GetPosY());
            int blockX = pos.x() / 70;
            int blockY = pos.y() / 70;

//            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
            if((blockX >= (pBlockX - 7)) && (blockX <= (pBlockX + 7)) && (blockY >= (pBlockY - 4)) && (blockY <= (pBlockY + 5)))
            {
                if(blockX <= pBlockX)
                    x = 7 - (pBlockX - blockX);
                else
                    x = 7 + (blockX - pBlockX);

//                x = blockX % 15;
                if(blockY <= pBlockY)
                    y = 5 - (pBlockY - blockY);
                else
                    y = 5 + (blockY - pBlockY);
//                y = blockY % 10/* + 1*/;

                if(y >= 0 && y < 10)
                {
                    if(obstacles.at(idx)->GetType() != NO_BLOCK_TYPE && obstacles.at(idx)->GetType() != GAP_BLOCK)
                    {
                        if(obstacles.at(idx)->GetType() == BONUS)
                            parsedView[y][x] = 5;
                        else if(obstacles.at(idx)->GetType() == GOAL || obstacles.at(idx)->GetType() == GOAL_MIDDLE || obstacles.at(idx)->GetType() == GOAL_BASE)
                            parsedView[y][x] = 6;
                        else
                            parsedView[y][x] = 1;

                    }
                }
            }

            // If we are looking at items out side the right most part of the screen,
            // stop searching through vector.
            // Note: This provides some improved performance near the start of the level
            //       (Not that any one will actually notice the improvement), but is
            //       reduced near the end of the level since the time complexity of the
            //       loop becomes O(n)
            //            if(pos.x() > view->width())
            if(blockX > (pBlockX + 7))
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
            int blockX = pos.x() / 70;
            int blockY = pos.y() / 70;

//            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
            if((blockX >= (pBlockX - 7)) && (blockX <= (pBlockX + 7)) && (blockY >= (pBlockY - 4)) && (blockY <= (pBlockY + 5)))
            {
                if(blockX <= pBlockX)
                    x = 7- (pBlockX - blockX);
                else
                    x = 7 + (blockX - pBlockX);

//                x = blockX % 15;
                if(blockY <= pBlockY)
                    y = 4 - (pBlockY - blockY);
                else
                    y = 5 + (blockY - pBlockY);
//                y = blockY % 10/* + 1*/;

                if(y >= 0 && y < 10)
                {
                    if(levelFloor.at(idx)->GetType() != NO_BLOCK_TYPE && levelFloor.at(idx)->GetType() != GAP_BLOCK && levelFloor.at(idx)->GetType() != INTERNAL_BLOCK && levelFloor.at(idx)->GetType() != FLOOR_COVERED_CORNER_LEFT && levelFloor.at(idx)->GetType() != FLOOR_COVERED_CORNER_RIGHT)
                        parsedView[y][x] = 1;
                }
            }

            // If we are looking at items out side the right most part of the screen,
            // stop searching through vector.
            // Note: This provides some improved performance near the start of the level
            //       (Not that any one will actually notice the improvement), but is
            //       reduced near the end of the level since the time complexity of the
            //       loop becomes O(n)
//            if(pos.x() > view->width())
            if(blockX > (pBlockX + 7))
                break;
        }
    }

    // Set the position of any enemies visible to the player
    foreach(QGraphicsPixmapItem* item, enemyItems)
    {
        if(enemies.at(enemyItems.indexOf(item)) != NULL)
        {
            int idx = enemyItems.indexOf(item);
            if(idx != -1 && enemies.at(idx)->isEnabled())
            {
                QPoint pos = view->mapFromScene(enemies.at(idx)->GetPosX(), enemies.at(idx)->GetPosY());
                int blockX = pos.x() / 70;
                int blockY = pos.y() / 70;

    //            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
                if((blockX >= (pBlockX - 7)) && (blockX <= (pBlockX + 7)) && (blockY >= (pBlockY - 4)) && (blockY <= (pBlockY + 5)))
                {
                    if(blockX <= pBlockX)
                        x = 7- (pBlockX - blockX);
                    else
                        x = 7 + (blockX - pBlockX);

    //                x = blockX % 15;
                    if(blockY <= pBlockY)
                        y = 4 - (pBlockY - blockY);
                    else
                        y = 5 + (blockY - pBlockY);
    //                y = blockY % 10/* + 1*/;

                    if((y+1) < 10 && parsedView[y+1][x] == 0)
                        y++;

                    if(y >= 0 && y < 10)
                        parsedView[y][x] = 3;
                }

                // If we are looking at items out side the right most part of the screen,
                // stop searching through vector.
                // Note: This provides some improved performance near the start of the level
                //       (Not that any one will actually notice the improvement), but is
                //       reduced near the end of the level since the time complexity of the
                //       loop becomes O(n)
//                if(pos.x() > view->width())
                if(blockX > (pBlockX + 7))
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
                    int blockX = pos.x() / 70;
                    int blockY = pos.y() / 70;

        //            if(pos.x() >= 0 && pos.x() <= view->width()&& pos.y() >= 0 && pos.y() <= view->height())
                    if((blockX >= (pBlockX - 7)) && (blockX <= (pBlockX + 7)) && (blockY >= (pBlockY - 4)) && (blockY <= (pBlockY + 5)))
                    {
                        if(blockX <= pBlockX)
                            x = 7 - (pBlockX - blockX);
                        else
                            x = 7 + (blockX - pBlockX);

        //                x = blockX % 15;
                        if(blockY <= pBlockY)
                            y = 4 - (pBlockY - blockY);
                        else
                            y = 5 + (blockY - pBlockY);
        //                y = blockY % 10/* + 1*/;

                        if(y >= 0 && y < 10)
                            parsedView[y][x] = 4;
                    }

                    // If we are looking at items out side the right most part of the screen,
                    // stop searching through vector.
                    // Note: This provides some improved performance near the start of the level
                    //       (Not that anywould will actually notice the improvement), but is
                    //       reduced near the end of the level since the time complexity of the
                    //       loop becomes O(n)
                    //            if(pos.x() > view->width())
                    if(blockX > (pBlockX + 7))
                        break;
                }
            }
        }
    }


}

void LevelBase::ResetLevel(GameView *view)
{
    qDebug() << "Resetting obstacles";
    foreach(BlockBase *bb, obstacles)
    {
        if(bb != NULL)
        {
            // Only reset the block if is a BonusBlock
            if(bb->isBonus())
            {
                ((BonusBlock*)bb)->SetHits(1);
                bb->SetType(NO_LEVEL_TYPE, BONUS);
                int idx = obstacles.indexOf(bb);
                obstacleItems.at(idx)->setPixmap(*obstacles.at(idx)->GetTexture());
                obstacleItems.at(idx)->setPos(bb->GetPosX(), bb->GetPosY());
            }
        }
    }

    qDebug() << "\tRemoving donut bounding boxes";
    foreach(QGraphicsPixmapItem *p, donutItems)
    {
        if(p != NULL)
        {
            if(p->scene() == view->GetScene())
                view->removePixmap(p);

            delete p;
        }
    }
    qDebug() << "Clearing DonutItems";
    if(donutItems.size() > 0)
        donutItems.clear();

    qDebug() << "Clearing donuts";
    if(donuts.size() > 0)
        donuts.clear();

    qDebug() << "Resetting Enemies";
    foreach(EnemyBase *eb, enemies)
    {
        if(!eb->isEnabled())
        {
            // Re-enable the enemy
            eb->Toggle();

            // Re-add the pixmap and bounding box the scene.
            int idx = enemyItems.indexOf(eb->GetGPixmapPtr());
            eb->SetGRectPtr(view->addRect(*eb->GetBoundingBox(), QPen(Qt::transparent)));
            eb->SetGPixmapPtr(view->addPixmap(*eb->GetTexture()));

            enemyItems.replace(idx, eb->GetGPixmapPtr());
            enemyItems.at(idx)->setPos(eb->GetPosX(), eb->GetPosY());
            enemyItems.at(idx)->setScale(0.5);

            enemyBBs.replace(idx, eb->GetGRectPtr());
            enemyBBs.at(idx)->setPos(0,0);
            enemyBBs.at(idx)->setScale(0.5);
        }
        else
        {
            // Toggle the enemy on and off to reset their position.
            eb->Toggle();
            eb->Toggle();
        }
    }

    nullHolder->ensureVisible(0,0,5,5,1,1);

    qDebug() << "--Done";
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

void LevelBase::SetLevelType(LevelType lt)
{
    foreach(QGraphicsPixmapItem *pi, floorItems)
    {
        int idx = floorItems.indexOf(pi);
        if(idx != -1)
        {
            if(levelFloor.at(idx)->GetLevelType() != NO_LEVEL_TYPE)
            {
                levelFloor.at(idx)->SetType(lt, levelFloor.at(idx)->GetType());
                pi->setPixmap(*levelFloor.at(idx)->GetTexture());
            }
        }
    }

    foreach(QGraphicsPixmapItem *pi, obstacleItems)
    {
        int idx = obstacleItems.indexOf(pi);
        if(idx != -1)
        {
            if(obstacles.at(idx)->GetLevelType() != NO_LEVEL_TYPE)
            {
                obstacles.at(idx)->SetType(lt, obstacles.at(idx)->GetType());
                pi->setPixmap(*obstacles.at(idx)->GetTexture());
            }
        }
    }
}

void LevelBase::ClearView(GameView *view)
{
    qDebug() << "\tRemoving Floor items";
    foreach(QGraphicsPixmapItem *i, floorItems)
    {
        if(i != NULL)
        {
            if(i->scene() == view->GetScene())
                view->removePixmap(i);

            delete i;
        }
    }
    floorItems.clear();

    qDebug() << "\tRemoving Obstacle items";
    foreach(QGraphicsPixmapItem *i, obstacleItems)
    {
        if(i != NULL)
        {
            if(i->scene() == view->GetScene())
                view->removePixmap(i);

            delete i;
        }
    }
    obstacleItems.clear();

    qDebug() << "\tRemoving enemy items";
    foreach(QGraphicsPixmapItem *i, enemyItems)
    {
        if(i != NULL)
        {
            if(i->scene() == view->GetScene())
                view->removePixmap(i);

            delete i;
        }
    }
    enemyItems.clear();

    qDebug() <<"\tRemoving floor bounding boxes";
    foreach(QGraphicsRectItem *r, floorBBs)
    {
        if(r != NULL)
        {
            if(r->scene() == view->GetScene())
                view->removeRect(r);

            delete r;
        }
    }
    floorBBs.clear();

    qDebug() << "\tRemoving obstacle bounding boxes";
    foreach(QGraphicsRectItem *r, obstacleBBs)
    {
        if(r != NULL)
        {
            if(r->scene() == view->GetScene())
                view->removeRect(r);

            delete r;
        }
    }
    obstacleBBs.clear();

    qDebug() << "\tRemoving enemy bounding boxes";
    foreach(QGraphicsRectItem *r, enemyBBs)
    {
        if(r != NULL)
        {
            if(r->scene() == view->GetScene())
                view->removeRect(r);

            delete r;
        }
    }
    enemyBBs.clear();

    qDebug() << "\tRemoving donut bounding boxes";
    foreach(QGraphicsPixmapItem *p, donutItems)
    {
        if(p != NULL)
        {
            if(p->scene() == view->GetScene())
                view->removePixmap(p);

            delete p;
        }
    }
    donutItems.clear();

//    foreach(EnemyBase *eb, enemies)
//    {
//        if(eb != NULL)
//            delete eb;
//    }
    enemies.clear();

//    foreach(BlockBase *bb, obstacles)
//    {
//        if(bb != NULL)
//            delete bb;
//    }
    obstacles.clear();

//    foreach(BlockBase *bb, levelFloor)
//    {
//        if(bb != NULL)
//            delete bb;
//    }
//    levelFloor.clear();

//    foreach(ItemBase *i, donuts)
//    {
//        if(i != NULL)
//            delete i;
//    }
    donuts.clear();

    view->ClearScene();
}

QVector<QVector<int> > LevelBase::GetParsedView()
{
    QVector<QVector<int>> tempVec;
    for(int i = 0; i<10;i++){
        QVector<int> innerVec;
        for(int j=0;j<18;j++){
           innerVec.push_back(parsedView[i][j]);
        }
        tempVec.push_back(innerVec);
    }

    return tempVec;
}//Draws the floor
