#include "itembase.h"

ItemBase::ItemBase()
{
    this->it = DONUT;
    SetItemInfo();
}

ItemBase::ItemBase(ItemType iType)
{
    this->it = iType;
    SetItemInfo();
}

void ItemBase::SetPostion(int x, int y)
{
    this->posX = x;
    this->posY = y;
}

QPixmap *ItemBase::GetTexture()
{
    return texture;
}

QRect *ItemBase::GetBoundingBox()
{
    return boundingBox;
}

QGraphicsPixmapItem *ItemBase::GetViewPixmap()
{
    return viewPixmap;
}

QGraphicsRectItem *ItemBase::GetViewBB()
{
    return viewBB;
}

void ItemBase::SetViewPixmap(QGraphicsPixmapItem *item)
{
    viewPixmap = item;
    viewPixmap->setScale(0.25);
}

void ItemBase::SetViewBB(QGraphicsRectItem *item)
{
    viewBB = item;
}

void ItemBase::SetItemInfo()
{
    if(it == DONUT)
    {
        texture = new QPixmap("Assets/Item/Donut.png");
        itemSize = Size{64, 64};
    }
    else
    {
        texture = new QPixmap("Assets/Item/one_up.png");
        itemSize = Size{64,64};
    }

    boundingBox = new QRect(posX, posY, itemSize.x, itemSize.y);
}
