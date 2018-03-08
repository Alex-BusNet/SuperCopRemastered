#include "donut.h"

Donut::Donut()
{

}

Donut::Donut(int posX, int posY)
{
    this->posX = posX;
    this->posY = posY;
    size = Size{20, 20};
    donut = new QPixmap("Assets/Donut/Donut.png");
    active = false;
}

Donut::~Donut()
{
    delete donut;
}

void Donut::drawDonut(QPainter &painter)
{
    painter.drawPixmap(posX, posY, size.x, size.y, *donut);
}

void Donut::changeImage(QString str)
{
    delete donut;
    donut = new QPixmap(str);
}

void Donut::setPosX(int x)
{
    posX = x;
}

void Donut::setPosY(int y)
{
    posY = y;
}

void Donut::setSize(Size s)
{
    size = s;
}

int Donut::getPosX()
{
    return posX;
}

int Donut::getPosY()
{
    return posY;
}

Size Donut::getSize()
{
    return size;
}

bool Donut::getActive()
{
    return active;
}

void Donut::setActive(bool act)
{
    active = act;
}
