#include "levelbase.h"

LevelBase::LevelBase()
{
    floor = new QPixmap("Assets/Wall/0.png");
    floorStart = 100;
    picX = 16;
    picY = 16;
    floorLength = 28.0;
    gameWindow = 100;

    generateLevel = new QTimer();
    generateLevel->setInterval(50);
}

LevelBase::LevelBase(int width, int height)
{
    floor = new QPixmap("Assets/Wall/0.png");
    floorStart = height - 100;
    picX = 16;
    picY = 16;
    floorLength = width / 28.0;
    gameWindow = width;

    generateLevel = new QTimer();
    generateLevel->setInterval(50);
}

LevelBase::~LevelBase()
{
    delete floor;
    delete generateLevel;
}//Destructor


void LevelBase::drawLevelBase(QPainter &painter)
{
    rect = new QRect(0, floorStart, gameWindow, 32);
    painter.drawRect(*rect);
    QBrush brush;
    brush.setTexture(*floor);
    painter.fillRect(*rect, brush);
}//Draws the floor
