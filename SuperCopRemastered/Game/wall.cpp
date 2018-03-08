#include "wall.h"

Wall::Wall()
{
    wall = new QPixmap("Assets/Wall/Box_2.png");

    wallPosX = 50;
    wallPosY = 140;
    picSize.x = 32;
    picSize.y = 40;
    active = false;
}

void Wall::drawWall(QPainter &painter)
{
    rectWall = new QRect(wallPosX, wallPosY, picSize.x, picSize.y);
    painter.drawRect(*rectWall);
    painter.drawTiledPixmap(*rectWall, *wall);
}

void Wall::setWallPosX(int x)
{
    this->wallPosX = x;
}//Mutator

void Wall::setWallPosY(int y)
{
    this->wallPosY = y;
}//Mutator

void Wall::setActive(bool active)
{
    this->active = active;
}//Mutator

int Wall::getWallPosX()
{
    return this->wallPosX;
}//Accessor

int Wall::getWallPosY()
{
    return this->wallPosY;
}

Size Wall::getWallSize()
{
    return picSize;
}//Accessor

bool Wall::isActive()
{
    return active;
}//Accessor
