#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QPixmap>
#include <QPainter>
#include <QRect>
#include <QString>
#include "datatypes.h"

#define LEVEL_SCROLL_SPEED 5 // px;

class BlockBase
{
public:
    BlockBase();
    BlockBase(LevelType lType, BlockType bType);

    void DrawBlock(QPainter &paint);
    void UpdateBlock(int playerDirection);

    void SetPosition(int x, int y);

    int GetPosX();
    int GetPosY();
    int GetXDim();
    int GetYDim();
    int GetRightEdge();
    Size GetSize();

    BlockType GetType();
    LevelType GetLevelType();

    QRect* GetBoundingBox();

    QPixmap* GetTexture();

private:
    LevelType lt;
    BlockType bt;
    Size blockSize; // px dimensions of image
    int xDim,yDim; // Number of blocks in X and Y direction
    int posX, posY;
    QRect *boundingBox;
    QPixmap *texture;

//    QString texturePath;

    void SetBlockInfo();
};

#endif // BLOCKBASE_H
