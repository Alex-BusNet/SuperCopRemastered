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
    this->boundLeft = 0;
    this->boundRight = 70 * 205;
    viewPixmap = NULL;
    viewBB = NULL;
    SetEnemyInfo();
}

void EnemyBase::DrawEnemy(QPainter &paint)
{
//    paint.drawPixmap(posX, posY, enemySize.x, enemySize.y, *texture);
}

void EnemyBase::UpdateEnemy()
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

    boundingBox = new QRect(posX, posY, enemySize.x, enemySize.y);

    if(viewBB != NULL)
        viewBB->setRect(*boundingBox);

//    viewBB->setRect(posX, posY, enemySize.x, enemySize.y);
//    viewBB->setPos(0, 0);
//    pixText->setPos(posX + 30, posY + 2);
//    bbText->setPos(posX + 30, posY + 12);
}

void EnemyBase::SetBounds(int left, int right)
{
    boundLeft = left;
    boundRight = right;
}

void EnemyBase::SetDirection(Direction dir)
{
    direction = dir;
}

void EnemyBase::FlipDirection()
{
    if(direction == WEST)
        direction = EAST;
    else
        direction = WEST;

    UpdateEnemy();
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

int EnemyBase::GetValue()
{
    return pointValue;
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

void EnemyBase::SetGPixmapText(QGraphicsTextItem *ptr)
{
    pixText = ptr;
}

void EnemyBase::SetGRectText(QGraphicsTextItem *ptr)
{
    bbText = ptr;
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
