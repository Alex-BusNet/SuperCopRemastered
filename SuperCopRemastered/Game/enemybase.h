#ifndef ENEMYBASE_H
#define ENEMYBASE_H

#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include "blockbase.h"

#include "datatypes.h"

class EnemyBase
{
public:
    EnemyBase();
    EnemyBase(int x, int y, EnemyType eType);

    void DrawEnemy(QPainter &paint);
    void UpdateEnemy();

    void SetBoundingObstacles(BlockBase *left, BlockBase *right);

private:

    void SetEnemyInfo();

    QString texturePath, color;
    QPixmap *texture;
    QRect *boundingBox;

    BlockBase *leftBound, *rightBound;

    int posX, posY;
    Size enemySize;

    int speed, frame;
    int pointValue;
    Direction direction;

    EnemyType et;

};

#endif // ENEMYBASE_H
