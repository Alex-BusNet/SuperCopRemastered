#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QPixmap>
#include <QPainter>
#include <QGraphicsRectItem>
#include <QRect>
#include <QString>
#include "datatypes.h"

#define LEVEL_SCROLL_SPEED 5 // px;

class BlockBase
{
public:
    BlockBase();
    BlockBase(LevelType lType, BlockType bType);
    ~BlockBase();

    void DrawBlock(QPainter &paint);
    void UpdateBlock(int playerDirection);

    void SetPosition(int x, int y);
    void SetType(LevelType lType, BlockType bType);

    int GetPosX();
    int GetPosY();
    int GetXDim();
    int GetYDim();
    int GetRightEdge();
    Size GetSize();
    QPointF GetMiddle();

    bool isBonus();

    BlockType GetType();
    LevelType GetLevelType();

    QRect* GetBoundingBox();
    QRect* GetLeftBoundingBox();
    QRect* GetRightBoundingBox();
    QRect* GetTopBoundingBox();
    QRect* GetBottomBoundingBox();

    void SetBottomGBB(QGraphicsRectItem* bb);
    void SetTopGBB(QGraphicsRectItem* bb);
    void SetLeftGBB(QGraphicsRectItem* bb);
    void SetRightGBB(QGraphicsRectItem* bb);

    QGraphicsRectItem* GetBottomGBB();
    QGraphicsRectItem* GetTopGBB();
    QGraphicsRectItem* GetLeftGBB();
    QGraphicsRectItem* GetRightGBB();

    QPixmap* GetTexture();

private:
    LevelType lt;
    BlockType bt;
    Size blockSize; // px dimensions of image
    int xDim,yDim; // Number of blocks in X and Y direction
    int posX, posY;
    QRect *boundingBox;
    QPixmap *texture;
    bool bonusType;

    QGraphicsRectItem *bottomBB, *topBB, *leftBB, *rightBB;

//    QString texturePath;

    void SetBlockInfo();
};

#endif // BLOCKBASE_H
