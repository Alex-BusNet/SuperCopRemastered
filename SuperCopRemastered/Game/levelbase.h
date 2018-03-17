#ifndef LEVELBASE_H
#define LEVELBASE_H

#include <QObject>
#include <QPainter>
#include <QBrush>
#include <QRect>
#include <QTimer>
#include <QVector>
#include <QPoint>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

#include "datatypes.h"
#include "gameview.h"
#include "blockbase.h"
#include "enemybase.h"
#include "player.h"

class LevelBase : public QObject
{
    Q_OBJECT
public:
    LevelBase();
    LevelBase(int width, int height);


    ~LevelBase();

    void LoadLevel(int levelNum, GameView *view);
    void drawLevelBase(QPainter &painter);

    void UpdateLevel(Player *p, GameView *view);

    int GetLevelRightBound();

    int getGround();
    QPoint GetPlayerStart();

    void ClearView(GameView *view);

private:
    // The floor of the level. A NO_LEVEL_TYPE represents a gap
    QVector<BlockBase*> levelFloor;
    // All non-floor objects in level.
    QVector<BlockBase*> obstacles;
    QVector<EnemyBase*> enemies;

    QVector<QGraphicsPixmapItem*> floorItems;
    QVector<QGraphicsPixmapItem*> obstacleItems;
    QVector<QGraphicsPixmapItem*> enemyItems;

    QVector<QGraphicsRectItem*> obstacleBBs;
    QVector<QGraphicsRectItem*> floorBBs;
    QVector<QGraphicsRectItem*> enemyBBs;

    QVector<QGraphicsTextItem*> enemyTextItems;
    QVector<QGraphicsTextItem*> enemyBBText;
    int floorHeight; // Y value for where to start drawing the floor (x=0)
    Size imageSize;

    // Defines the vector indices that should be rendered
    int leftWindowBound, rightWindowBound;
    int leftObstacleBound, rightObstacleBound;
    int leftEnemyBound, rightEnemyBound;
    int windowWidth;
//    int posIdx;
    int collidedItems, feetItems;
//    int pxPerItem;
//    int pXCenter, pYFeet;

    QPixmap *floor;
    QRect *rect;

    bool updateStatus;

    QPoint playerStart;

signals:
    void EnemyDefeated(int value);
    void EndOfGame(bool endOfLevel);

public slots:
    void generatePiece();
};

#endif // LEVELBASE_H
