#include "blockbase.h"
#include <QDebug>

BlockBase::BlockBase()
{
    this->lt = GRASS;
    this->bt = BLOCK;
    SetBlockInfo();
}

BlockBase::BlockBase(LevelType lType, BlockType bType)
{
    this->lt = lType;
    this->bt = bType;
    SetBlockInfo();
}

void BlockBase::DrawBlock(QPainter &paint)
{
    for(int i = 0; i < x; i++)
    {
        for(int j = 0; j < y; j++)
        {
            paint.drawPixmap(posX + (blockSize.x * i), posY + (blockSize.y * j), blockSize.x, blockSize.y, *texture);
        }
    }

    paint.drawRect(QRect(posX, posY, blockSize.x * x, blockSize.y * y));
}

void BlockBase::UpdateBlock(int playerDirection)
{
    if(playerDirection == EAST)
        posX -= LEVEL_SCROLL_SPEED;
    else
        posX += LEVEL_SCROLL_SPEED;
}

void BlockBase::SetPosition(int x, int y)
{
    this->posX = x;
    this->posY = y;
}

int BlockBase::GetPosX()
{
    return posX;
}

int BlockBase::GetPosY()
{
    return posY;
}

int BlockBase::GetRightEdge()
{
    return posX + blockSize.x;
}

Size BlockBase::GetSize()
{
    return blockSize;
}

void BlockBase::SetBlockInfo()
{
    switch(lt)
    {
    case GRASS:
        texturePath = QString("Assets/Block/Grass");
        break;
    case INDUSTRIAL:
        texturePath = QString("Assets/Block/Industrial");
        break;
    case MEDIEVIL:
        texturePath = QString("Assets/Block/Medievil");
        break;
    case CAVE:
        texturePath = QString("Assets/Block/Caves");
        break;
    case NO_LEVEL_TYPE: // Used only with BonusBlock objects and gaps in floor
        texturePath = QString("Assets");
        break;
    }

    blockSize = Size{35, 35};

    switch(bt)
    {
    case BLOCK:
        x = y = 1;
        texture = new QPixmap(texturePath + "/edge_block.png");
        break;
    case SHORT_PLATFORM:
        x = 3;
        y = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case MEDIUM_PLATFORM:
        x = 4;
        y = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case LONG_PLATFORM:
        x = 5;
        y = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case SHORT_WALL:
        x = 2;
        y = 2;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_WALL:
        x = 2;
        y = 4;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case SHORT_DEEP_WALL:
        x = 4;
        y = 2;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_DEEP_WALL:
        x = 4;
        y = 4;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case SHORT_STAIR:
        x = 3;
        y = 3;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_STAIR:
        x = 6;
        y = 6;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case GOAL:
        x = 1;
        y = 3;
        texture = new QPixmap(texturePath + "/Goal/goal_empty_top.png");
        break;
    case BONUS:
        x = y = 1;
        texture = new QPixmap(texturePath + "/Block/bonus_block.png");
        break;
    }

    texture->scaled(blockSize.x, blockSize.y);
}
