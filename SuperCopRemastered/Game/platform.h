#ifndef PLATFORM_H
#define PLATFORM_H

#include <QPainter>
#include <QWidget>
#include <QRect>

#include "datatypes.h"

class Platform
{
public:
    Platform();
    Platform(int baseHeight, int windowWidth);
    ~Platform();

    void drawPlatform(QPainter &painter);
    void setPlatformPosX(int x);
    void setActive(bool active);

    int getPlatformPosX();
    int getPlatformPosY();
    int getPlatformEnd();
    Size getPlatformSize();
    bool isActive();

private:
    QPixmap *platform;
    QRect *rectPlat;
    int picX, picY;
    int platformHeight, platformPosX, platformLength;
    int gameWindow;
    bool active;
};

#endif // PLATFORM_H
