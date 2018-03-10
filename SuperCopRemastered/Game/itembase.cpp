#include "itembase.h"

ItemBase::ItemBase()
{
    this->it = DONUT;
}

ItemBase::ItemBase(ItemType iType)
{
    this->it = iType;
}

void ItemBase::SetPostion(int x, int y)
{
    this->posX = x;
    this->posY = y;
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
}
