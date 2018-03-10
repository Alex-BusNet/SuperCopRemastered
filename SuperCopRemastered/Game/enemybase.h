#ifndef ENEMYBASE_H
#define ENEMYBASE_H

#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QString>

#include "datatypes.h"

class EnemyBase
{
public:
    EnemyBase();
    EnemyBase(int x, int y, EnemyType eType);

    void DrawEnemy(QPainter &paint);

private:

    void SetEnemyInfo();

    QString texturePath, color;
    QPixmap *texture;
    QRect *boundingBox;

    int posX, posY;
    Size enemySize;

    int speed, frame;
    int pointValue;
    Direction direction;

    EnemyType et;

};

#endif // ENEMYBASE_H
