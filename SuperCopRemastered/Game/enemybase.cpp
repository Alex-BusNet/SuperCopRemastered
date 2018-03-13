#include "enemybase.h"

EnemyBase::EnemyBase()
{

}

EnemyBase::EnemyBase(int x, int y, EnemyType eType)
{
    this->et = eType;
    this->posX = x;
    this->posY = y;
    this->enemySize = Size{35, 35};
    SetEnemyInfo();
}

void EnemyBase::DrawEnemy(QPainter &paint)
{
    paint.drawPixmap(posX, posY, enemySize.x, enemySize.y, *texture);
}

void EnemyBase::UpdateEnemy()
{
    frame = ((frame + 1) % 2) + 1;
    texture = new QPixmap(texturePath.arg(color).arg(QString::number(frame)));

    if(direction == EAST)
        posX += speed;
    else
        posX -= speed;

    // Adjust the Direction of the enemy;
    if((posX + enemySize.x) >= rightBound->GetPosX())
    {
        direction = WEST;
        posX = leftBound->GetRightEdge();
    }
    else if(posX <= (leftBound->GetPosX() + leftBound->GetSize().y))
    {
        direction = EAST;
        posX = rightBound->GetPosX();
    }
}

void EnemyBase::SetBoundingObstacles(BlockBase *left, BlockBase *right)
{
    leftBound = left;
    rightBound = right;
}

void EnemyBase::SetEnemyInfo()
{
    texturePath = QString("Assets/Enemy/robot_%1Drive%2.png");

    switch(et)
    {
    case NORMAL:
        speed = 5; // px per update
        pointValue = 10;
        direction = WEST;
        color = "green";
        texture = new QPixmap(texturePath.arg(color).arg("1"));
        break;
    case FAST:
        speed = 10;
        pointValue = 15;
        direction = WEST;
        color = "red";
        texture = new QPixmap(texturePath.arg(color).arg("1"));
        break;
    case JUMPER:
        speed = 5;
        pointValue = 20;
        direction = WEST;
        color = "yellow";
        texture = new QPixmap(texturePath.arg(color).arg("1"));
        break;
    case TURRET:
        speed = 3;
        pointValue = 25;
        color = "blue";
        texture = new QPixmap(texturePath.arg(color).arg("1"));
        break;
    }

    direction = WEST;
    frame = 1;
}
