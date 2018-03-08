#include "platform.h"

Platform::Platform()
{
    platform = new QPixmap("Assets/Wall/Box_2.png");
    platformHeight = 160;
    platformPosX = 20;
    platformLength = 48;
    active = false;
}

Platform::Platform(int baseHeight, int windowWidth)
{
    platform = new QPixmap("Assets/Wall/Box_2.png");
    platformHeight = baseHeight - 160;
    platformPosX = windowWidth + 20;
    platformLength = 48;
    active = false;
}

Platform::~Platform()
{

}

void Platform::drawPlatform(QPainter &painter)
{
    rectPlat = new QRect(platformPosX, platformHeight, platformLength, 16);
    painter.drawRect(*rectPlat);
    painter.drawTiledPixmap(*rectPlat, *platform);
}

void Platform::setPlatformPosX(int x)
{
    this->platformPosX = x;
}

void Platform::setActive(bool active)
{
    this->active = active;
}

int Platform::getPlatformPosX()
{
    return platformPosX;
}

int Platform::getPlatformPosY()
{
    return platformHeight;
}

int Platform::getPlatformEnd()
{
    return platformPosX + platformLength;
}

Size Platform::getPlatformSize()
{
    return Size{platformLength, platformHeight};
}

bool Platform::isActive()
{
    return active;
}
