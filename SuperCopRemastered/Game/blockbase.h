#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include <QPixmap>
#include <QPainter>
#include <QRect>
#include <QString>
#include "datatypes.h"

class BlockBase
{
public:
    BlockBase();
    BlockBase(LevelType lType, BlockType bType);

    void DrawBlock(QPainter &paint);
    void SetPosition(int x, int y);
private:
    LevelType lt;
    BlockType bt;
    Size blockSize; // px dimensions of image
    int x,y; // Number of blocks in X and Y direction
    int posX, posY;
    QRect *boundingBox;
    QPixmap *texture;

    QString texturePath;

    void SetBlockInfo();
};

#endif // BLOCKBASE_H
