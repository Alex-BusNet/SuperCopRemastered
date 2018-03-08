#include "enemy.h"

Enemy::Enemy()
{
    posX = 20;
    posY = 140;
    size = Size{40,40};
    robot = new QPixmap("Assets/Enemy/Enemy0.png");
    active = false;
    direction = 0;
}

Enemy::~Enemy()
{
    if(robot != NULL)
        delete robot;
}

void Enemy::drawEnemy(QPainter &painter)
{
    painter.drawPixmap(posX, posY, size.x, size.y, *robot);
}

void Enemy::changeImage(QString str)
{
    delete robot;
    robot = new QPixmap(str);
}

void Enemy::setPosX(int x)
{
    posX = x;
}

void Enemy::setPosY(int y)
{
    posY = y;
}

void Enemy::setSizeX(int x)
{
    size.x = x;
}

void Enemy::setSizeY(int y)
{
    size.y = y;
}

int Enemy::getPosX()
{
    return posX;
}

int Enemy::getPosY()
{
    return posY;
}

Size Enemy::getSize()
{
    return size;
}

bool Enemy::getActive()
{
    return active;
}

void Enemy::setActive(bool act)
{
    this->active = act;
}

void Enemy::setDirection(int direc)
{
    this->direction = direc;
}

int Enemy::getDirection()
{
    return direction;
}
