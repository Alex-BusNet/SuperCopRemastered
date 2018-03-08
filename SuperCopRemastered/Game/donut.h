#ifndef DONUT_H
#define DONUT_H

#include <QPainter>
#include <QPixmap>
#include <QWidget>

#include "datatypes.h"

class Donut
{
public:
    Donut();
    Donut(int posX, int posY);
    ~Donut();

    void drawDonut(QPainter &painter);
    void changeImage(QString str);

    void setPosX(int x);
    void setPosY(int y);
    void setSize(Size s);

    int getPosX();
    int getPosY();
    Size getSize();

    bool getActive();
    void setActive(bool act);

private:
    int windowwidth;
    int posX;
    int posY;
    Size size;
    QPixmap *donut;
    bool active;
};

#endif // DONUT_H
