#include "enemybase.h"
#include <QDebug>

EnemyBase::EnemyBase()
{

}

EnemyBase::EnemyBase(int x, int y, EnemyType eType)
{
    this->et = eType;
    this->posX = x;
    this->posY = y;
    this->enemySize = Size{158, 120};
    this->boundingBox = new QRect(posX, posY, enemySize.x, enemySize.y);
    SetEnemyInfo();
}

void EnemyBase::DrawEnemy(QPainter &paint)
{
//    paint.drawPixmap(posX, posY, enemySize.x, enemySize.y, *texture);
}

void EnemyBase::UpdateEnemy(PlayerState playerState)
{
    frame = ((frame + 1) % 2) + 1;

    if(direction == EAST)
    {
        posX += speed;
    }
    else if(direction == WEST)
    {
        posX -= speed;
    }

    // Adjust the Direction of the enemy;
    if(direction == EAST)
    {
        if((posX + enemySize.x) >= boundRight)
        {
            direction = WEST;
        }
    }

    if(direction == WEST)
    {
        if(posX <= boundLeft)
        {
            direction = EAST;
        }
    }

    texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(QString::number(frame)));

    viewPixmap->setPos(posX, posY);
    viewPixmap->setPixmap(*texture);
    viewBB->setPos(posX, posY);
}

void EnemyBase::SetBounds(int left, int right)
{
    boundLeft = left;
    boundRight = right;
}

int EnemyBase::GetPosX()
{
    return posX;
}

int EnemyBase::GetPosY()
{
    return posY;
}

int EnemyBase::GetRightEdge()
{
    return posX + enemySize.x;
}

QPixmap *EnemyBase::GetTexture()
{
    return texture;
}

QRect *EnemyBase::GetBoundingBox()
{
    return boundingBox;
}

void EnemyBase::SetGPixmapPtr(QGraphicsPixmapItem *ptr)
{
    viewPixmap = ptr;
}

void EnemyBase::SetGRectPtr(QGraphicsRectItem *ptr)
{
    viewBB = ptr;
}

QGraphicsPixmapItem *EnemyBase::GetGPixmapPtr()
{
    return viewPixmap;
}

QGraphicsRectItem *EnemyBase::GetGRectPtr()
{
    return viewBB;
}

void EnemyBase::SetEnemyInfo()
{
    texturePath = QString("Assets/Enemy/%1/robot_%2Drive%3.png");

    switch(et)
    {
    case NORMAL:
        speed = 15; // px per update
        pointValue = 10;
        direction = WEST;
        color = "green";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg("1"));
        break;
    case FAST:
        speed = 25;
        pointValue = 15;
        direction = WEST;
        color = "red";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg("1"));
        break;
    case JUMPER:
        speed = 5;
        pointValue = 20;
        direction = WEST;
        color = "yellow";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg("1"));
        break;
    case TURRET:
        speed = 3;
        pointValue = 25;
        color = "blue";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg("1"));
        break;
    }

    direction = WEST;
    frame = 1;
}
