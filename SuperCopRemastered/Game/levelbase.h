#ifndef LEVELBASE_H
#define LEVELBASE_H

#include <QPainter>
#include <QRect>
#include <QTimer>

class LevelBase
{
public:
    LevelBase();
    LevelBase(int width, int height);
    ~LevelBase();

    void drawLevelBase(QPainter &painter);

private:
    QPixmap *floor;
    int floorStart;
    double floorLength;
    int floorHeight;
    int picX;
    int picY;
    int gameWindow;
    QRect *rect;
    QTimer *generateLevel;

public slots:
    void generatePiece();
};

#endif // LEVELBASE_H
