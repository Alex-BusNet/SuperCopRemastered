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
#include "bonusblock.h"
#include "enemybase.h"
#include "itembase.h"
#include "player.h"

class LevelBase : public QObject
{
    Q_OBJECT
public:
    LevelBase();
    LevelBase(int width, int height);


    ~LevelBase();

    void LoadLevel(int levelNum, GameView *view, bool devMode);
    void drawLevelBase(QPainter &painter, bool devMode);

    void UpdateLevel(Player *p, GameView *view, bool devMode);

    int GetLevelRightBound();

    void SetLevelType(LevelType lt);

    int getGround();
    QPoint GetPlayerStart();

    void ClearView(GameView *view);

    QVector<QVector<int>> GetParsedView();

private:
    // The floor of the level. A NO_LEVEL_TYPE represents a gap
    QVector<BlockBase*> levelFloor;
    // All non-floor objects in level.
    QVector<BlockBase*> obstacles;
    QVector<EnemyBase*> enemies;
    QVector<ItemBase*> donuts;
    QVector<QGraphicsPixmapItem*> floorItems;
    QVector<QGraphicsPixmapItem*> obstacleItems;
    QVector<QGraphicsPixmapItem*> enemyItems;
    QVector<QGraphicsPixmapItem*> donutItems;

    QVector<QGraphicsRectItem*> obstacleBBs;
    QVector<QGraphicsRectItem*> floorBBs;
    QVector<QGraphicsRectItem*> enemyBBs;
    QVector<QGraphicsRectItem*> donutBBs;

    QVector<QGraphicsTextItem*> enemyTextItems;
    QVector<QGraphicsTextItem*> enemyBBText;


    Size imageSize;

    int floorHeight; // Y value for where to start drawing the floor (x=0)
    int windowWidth;
    int collidedItems, feetItems;

    QPixmap *floor;
    QRect *rect;


    QGraphicsRectItem *nullHolder;
    bool updateStatus;

    QPoint playerStart;
    int parsedView[10][18]; // [y][x]

signals:
    void EnemyDefeated(int value);
    void EndOfGame(bool endOfLevel);

};

#endif // LEVELBASE_H
