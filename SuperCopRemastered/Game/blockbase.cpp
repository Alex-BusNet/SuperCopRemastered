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
    for(int i = 0; i < xDim; i++)
    {
        for(int j = 0; j < yDim; j++)
        {
            paint.drawPixmap(posX + (blockSize.x * i), posY + (blockSize.y * j), blockSize.x, blockSize.y, *texture);
        }
    }

    paint.drawRect(QRect(posX, posY, blockSize.x * xDim, blockSize.y * yDim));
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
    boundingBox = new QRect(posX, posY, blockSize.x * xDim, blockSize.y * yDim);
}

int BlockBase::GetPosX()
{
    return posX;
}

int BlockBase::GetPosY()
{
    return posY;
}

int BlockBase::GetXDim()
{
    return xDim;
}

int BlockBase::GetYDim()
{
    return yDim;
}

int BlockBase::GetRightEdge()
{
    return posX + (blockSize.x * xDim);
}

Size BlockBase::GetSize()
{
    return blockSize;
}

QRect *BlockBase::GetBoundingBox()
{
    return boundingBox;
}

QPixmap *BlockBase::GetTexture()
{
    return texture;
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

    blockSize = Size{70, 70};

    switch(bt)
    {
    case BLOCK:
        xDim = yDim = 1;
        texture = new QPixmap(texturePath + "/edge_block.png");
        break;
    case SHORT_PLATFORM:
        xDim = 3;
        yDim = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case MEDIUM_PLATFORM:
        xDim = 4;
        yDim = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case LONG_PLATFORM:
        xDim = 5;
        yDim = 1;
        texture = new QPixmap(texturePath + "/platform_mid.png");
        break;
    case SHORT_WALL:
        xDim = 2;
        yDim = 2;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_WALL:
        xDim = 2;
        yDim = 4;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case SHORT_DEEP_WALL:
        xDim = 4;
        yDim = 2;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_DEEP_WALL:
        xDim = 4;
        yDim = 4;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case SHORT_STAIR:
        xDim = 3;
        yDim = 3;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case TALL_STAIR:
        xDim = 6;
        yDim = 6;
        texture = new QPixmap(texturePath + "/internal_block.png");
        break;
    case GOAL:
        xDim = 1;
        yDim = 3;
        texture = new QPixmap(texturePath + "/Goal/goal_empty_top.png");
        break;
    case BONUS:
        xDim = yDim = 1;
        texture = new QPixmap(texturePath + "/Block/bonus_block.png");
        break;
    }


    texture->scaled(blockSize.x, blockSize.y);
}
