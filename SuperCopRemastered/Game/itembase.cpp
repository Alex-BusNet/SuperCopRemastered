#include "itembase.h"

ItemBase::ItemBase()
{
    this->it = DONUT;

    boundingBox = NULL;
    texture = NULL;
    viewPixmap = NULL;
    viewBB = NULL;

    SetItemInfo();
}

ItemBase::ItemBase(ItemType iType)
{
    this->it = iType;

    boundingBox = NULL;
    texture = NULL;
    viewPixmap = NULL;
    viewBB = NULL;

    SetItemInfo();
}

ItemBase::~ItemBase()
{
//    if(boundingBox != NULL)
//        delete boundingBox;

    if(texture != NULL)
        delete texture;

    if(viewPixmap != NULL)
        delete viewPixmap;

    if(viewBB != NULL)
        delete viewBB;
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
    if(viewPixmap != NULL)
        delete viewPixmap;

    viewPixmap = item;
    viewPixmap->setScale(0.25);
}

void ItemBase::SetViewBB(QGraphicsRectItem *item)
{
    if(viewBB != NULL)
        delete viewBB;

    viewBB = item;
}

int ItemBase::GetPosX()
{
    return posX;
}

int ItemBase::GetPosY()
{
    return posY;
}

void ItemBase::SetItemInfo()
{
    if(texture != NULL)
        delete texture;

    if(boundingBox != NULL)
        delete boundingBox;

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
