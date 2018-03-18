#ifndef ITEMBASE_H
#define ITEMBASE_H

#include <QPainter>
#include <QString>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QRect>

#include "datatypes.h"

class ItemBase
{
public:
    ItemBase();
    ItemBase(ItemType iType);

    void SetPostion(int x, int y);

    QPixmap* GetTexture();
    QRect* GetBoundingBox();

    QGraphicsPixmapItem* GetViewPixmap();
    QGraphicsRectItem* GetViewBB();

    void SetViewPixmap(QGraphicsPixmapItem *item);
    void SetViewBB(QGraphicsRectItem *item);

private:
    ItemType it;
    Size itemSize;  // px dimesions
    int posX, posY; // Item position relative to level

    QPixmap *texture;
    QRect *boundingBox;

    QGraphicsRectItem* viewBB;
    QGraphicsPixmapItem* viewPixmap;

    void SetItemInfo();
};

#endif // ITEMBASE_H
