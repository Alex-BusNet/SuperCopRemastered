#ifndef ENEMY_H
#define ENEMY_H

#include <QPainter>
#include <QPixmap>
#include <QWidget>

#include "datatypes.h"

class Enemy
{
public:
    Enemy();
    ~Enemy();

    void drawEnemy(QPainter &painter);
    void changeImage(QString str);

    void setPosX(int x);
    void setPosY(int y);
    void setSizeX(int x);
    void setSizeY(int y);

    int getPosX();
    int getPosY();
    Size getSize();

    bool getActive();
    void setActive(bool act);
    void setDirection(int direc);
    int getDirection();

private:
    int posX;
    int posY;
    Size size;
    int frame;
    QPixmap *robot;
    bool active;
    int direction;

    enum enemyDirection {LEFT = 0, RIGHT = 1};
};

#endif // ENEMY_H
