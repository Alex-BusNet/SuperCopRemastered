#include "enemybase.h"
#include <QDebug>

EnemyBase::EnemyBase() : EnemyBase(0, 0, NORMAL)
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
    this->texturePath = QString("Assets/Enemy/%1/robot_%2Drive%3.png");

    viewPixmap = NULL;
    viewBB = NULL;
    texture = NULL;

    this->enabled = true;

    SetEnemyInfo();
}

EnemyBase::~EnemyBase()
{
//    if(boundingBox != NULL)
//        delete boundingBox;

    if(texture != NULL)
        delete texture;

    if(viewBB != NULL)
        delete viewBB;

    if(viewPixmap != NULL)
        delete viewPixmap;
}

void EnemyBase::DrawEnemy(QPainter &paint)
{
//    paint.drawText(90, 220, QString("Enemy Frame Count: %1").arg(frame));
}

void EnemyBase::UpdateEnemy()
{
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

    frame++;

    if(texture != NULL)
        delete texture;

    texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));

    if(viewPixmap != NULL)
    {
        viewPixmap->setPixmap(*texture);
        viewPixmap->setPos(posX, posY);
    }

    if(frame >= ANIM_FRAME_COUNT)
        frame = 0;

    if(boundingBox != NULL)
        delete boundingBox;

    boundingBox = new QRect(posX, posY, enemySize.x, enemySize.y);

    if(viewBB != NULL)
    {
        viewBB->setRect(posX, posY, enemySize.x, enemySize.y);
        viewBB->setPos(posX / 2, posY / 2);
    }
}

void EnemyBase::SetBounds(int left, int right)
{
    boundLeft = left;
    boundRight = right;
}

void EnemyBase::SetStartParams(int x, int y, Direction dir)
{
    this->startDir = dir;
    this->startX = x;
    this->startY = y;
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

void EnemyBase::Toggle()
{
    this->enabled = !this->enabled;

    if(enabled)
    {
        posX = startX;
        posY = startY;
        direction = startDir;
        frame = 0;

        if(texture != NULL)
            delete texture;

        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));

        if(viewPixmap != NULL)
        {
            viewPixmap->setPixmap(*texture);
            viewPixmap->setPos(posX, posY);
        }

        if(frame >= ANIM_FRAME_COUNT)
            frame = 0;

        if(boundingBox != NULL)
            delete boundingBox;

        boundingBox = new QRect(posX, posY, enemySize.x, enemySize.y);

        if(viewBB != NULL)
        {
            viewBB->setRect(posX, posY, enemySize.x, enemySize.y);
            viewBB->setPos(posX / 2, posY / 2);
        }
    }
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

bool EnemyBase::isEnabled()
{
    return this->enabled;
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
    if(viewPixmap != NULL)
        delete viewPixmap;

    viewPixmap = ptr;
}

void EnemyBase::SetGRectPtr(QGraphicsRectItem *ptr)
{
    if(viewBB != NULL)
        delete viewBB;

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
    direction = WEST;
    frame = 1;

    if(texture != NULL)
        delete texture;

    switch(et)
    {
    case NORMAL:
        speed = 5; // px per update
        pointValue = 10;
        direction = WEST;
        color = "green";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));
        break;
    case FAST:
        speed = 10;
        pointValue = 15;
        direction = WEST;
        color = "red";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));
        break;
    case JUMPER:
        speed = 5;
        pointValue = 20;
        direction = WEST;
        color = "yellow";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));
        break;
    case TURRET:
        speed = 3;
        pointValue = 25;
        color = "blue";
        texture = new QPixmap(texturePath.arg(direction == WEST ? "Left" : "Right" ).arg(color).arg(frame));
        break;
    }

}
