#ifndef ENEMYBASE_H
#define ENEMYBASE_H

#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include "blockbase.h"

#include "datatypes.h"

#define ANIM_FRAME_COUNT 2

class EnemyBase
{
public:
    EnemyBase();
    EnemyBase(int x, int y, EnemyType eType);
    ~EnemyBase();

    void DrawEnemy(QPainter &paint);
    void UpdateEnemy();

    void SetBounds(int left, int right);
    void SetStartParams(int x, int y, Direction dir);
    void SetDirection(Direction dir);
    void FlipDirection();
    void Toggle();

    int GetPosX();
    int GetPosY();
    int GetRightEdge();
    Size GetSize();
    QPoint GetMiddle();

    bool isEnabled();

    int GetValue();

    QPixmap* GetTexture();
    QRect* GetBoundingBox();

    void SetGPixmapPtr(QGraphicsPixmapItem *ptr);
    void SetGRectPtr(QGraphicsRectItem *ptr);

    QGraphicsPixmapItem* GetGPixmapPtr();
    QGraphicsRectItem* GetGRectPtr();

private:

    void SetEnemyInfo();

    QString texturePath, color;
    QPixmap *texture;
    QRect *boundingBox;

    QGraphicsPixmapItem *viewPixmap;
    QGraphicsRectItem *viewBB;

    int posX, posY;
    int startX, startY;

    int boundLeft, boundRight;
    Size enemySize;

    int speed, frame;
    int pointValue;

    Direction direction;
    Direction startDir;

    bool enabled;

    EnemyType et;

};

#endif // ENEMYBASE_H
