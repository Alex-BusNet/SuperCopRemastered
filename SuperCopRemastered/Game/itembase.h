#ifndef ITEMBASE_H
#define ITEMBASE_H

#include <QPainter>
#include <QString>
#include <QPixmap>
#include <QRect>
#include "datatypes.h"

class ItemBase
{
public:
    ItemBase();
    ItemBase(ItemType iType);

    void SetPostion(int x, int y);

private:
    ItemType it;
    Size itemSize;  // px dimesions
    int posX, posY; // Item position relative to level
    QPixmap *texture;

    void SetItemInfo();
};

#endif // ITEMBASE_H
