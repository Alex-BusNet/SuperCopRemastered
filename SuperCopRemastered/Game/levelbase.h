#ifndef LEVELBASE_H
#define LEVELBASE_H

#include <QPainter>
#include <QBrush>
#include <QRect>
#include <QTimer>
#include <QVector>

#include "datatypes.h"
#include "blockbase.h"
#include "enemybase.h"

class LevelBase
{
public:
    LevelBase();
    LevelBase(int width, int height);


    ~LevelBase();

    void LoadLevel(int levelNum);
    void drawLevelBase(QPainter &painter);

    void ShiftBoundsForward();
    void ShiftBoundsReverse();

private:
    // The floor of the level. A NO_LEVEL_TYPE represents a gap
    QVector<BlockBase*> levelFloor;
    // All non-floor objects in level.
    QVector<BlockBase*> obstacles;

    QVector<EnemyBase*> enemies;

    int floorHeight; // Y value for where to start drawing the floor (x=0)
    Size imageSize;

    // Defines the vector indices that should be rendered
    int leftWindowBound, rightWindowBound;
    int leftObstacleBound, rightObstacleBound;
    int leftEnemyBound, rightEnemyBound;

    QPixmap *floor;
    QRect *rect;

public slots:
    void generatePiece();
};

#endif // LEVELBASE_H
