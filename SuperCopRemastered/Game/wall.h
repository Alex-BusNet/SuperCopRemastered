#ifndef WALL_H
#define WALL_H

#include <QPixmap>
#include <QPainter>
#include "datatypes.h"

class Wall
{
public:
    Wall();
    ~Wall();

    void drawWall(QPainter &painter);

    void setWallPosX(int x);
    void setWallPosY(int y);
    void setActive(bool active);

    int getWallPosX();
    int getWallPosY();
    Size getWallSize();
    int getWallType();

    bool isActive();

private:
    QPixmap *wall;
    QRect *rectWall;
    Size picSize;
    int gameWindow;
    int wallPosX, wallPosY;
    bool  active;
};

#endif // WALL_H
