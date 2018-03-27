#include "blockbase.h"
#include <QDebug>

BlockBase::BlockBase()
{
    this->lt = GRASS;
    this->bt = BLOCK;
    boundingBox = NULL;
    SetBlockInfo();
}

BlockBase::BlockBase(LevelType lType, BlockType bType)
{
    this->lt = lType;
    this->bt = bType;
    boundingBox = NULL;
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

void BlockBase::SetType(LevelType lType, BlockType bType)
{
    lt = lType;
    bt = bType;
    SetBlockInfo();
//    qDebug() << "New Block type: " << bt;
    //    qDebug() << "New Level type: " << lt;
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

BlockType BlockBase::GetType()
{
    return bt;
}

LevelType BlockBase::GetLevelType()
{
    return lt;
}

QRect *BlockBase::GetBoundingBox()
{
    return boundingBox;
}

QRect *BlockBase::GetLeftBoundingBox()
{
    return new QRect(boundingBox->x() - 5, boundingBox->y() + 10, 10, boundingBox->height() - 20);
}

QRect *BlockBase::GetRightBoundingBox()
{
    return new QRect(boundingBox->x() + boundingBox->width() - 5, boundingBox->y() + 10, 10, boundingBox->height() - 20);
}

QRect *BlockBase::GetTopBoundingBox()
{
    return new QRect(boundingBox->x(), boundingBox->y() - 20, boundingBox->width(), 40);
}

QRect *BlockBase::GetBottomBoundingBox()
{
    return new QRect(boundingBox->x()+5, boundingBox->y() + boundingBox->height() - 20, boundingBox->width() - 10, 30);
}

void BlockBase::SetBottomGBB(QGraphicsRectItem *bb)
{
    bottomBB = bb;
}

void BlockBase::SetTopGBB(QGraphicsRectItem *bb)
{
    topBB = bb;
}

void BlockBase::SetLeftGBB(QGraphicsRectItem *bb)
{
    leftBB = bb;
}

void BlockBase::SetRightGBB(QGraphicsRectItem *bb)
{
    rightBB = bb;
}

QGraphicsRectItem *BlockBase::GetBottomGBB()
{
    return bottomBB;
}

QGraphicsRectItem *BlockBase::GetTopGBB()
{
    return topBB;
}

QGraphicsRectItem *BlockBase::GetLeftGBB()
{
    return leftBB;
}

QGraphicsRectItem *BlockBase::GetRightGBB()
{
    return rightBB;
}

QPixmap *BlockBase::GetTexture()
{
    return texture;
}

void BlockBase::SetBlockInfo()
{
    QString texturePath;

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
    case NO_LEVEL_TYPE: // Used only with BonusBlock objects, gaps in floor, and the goal
        texturePath = QString("Assets/Block/Special");
        break;
    default:
        break;
    }

    blockSize = Size{70, 70};

    switch(bt)
    {
    case BLOCK:
        texturePath += "/block.png";
        break;
    case BLOCK_EDGE_TOP:
        texturePath += "/block_edge_top.png";
        break;
    case WALL_CORNER_LEFT:
        texturePath += "/wall_corner_left.png";
        break;
    case WALL_CORNER_RIGHT:
        texturePath += "/wall_corner_right.png";
        break;
    case WALL_SIDE_LEFT:
        texturePath += "/wall_side_left.png";
        break;
    case WALL_SIDE_RIGHT:
        texturePath += "/wall_side_right.png";
        break;
    case INTERNAL_BLOCK:
        texturePath += "/internal_block.png";
        break;
    case PLATFORM_LEFT:
        texturePath += "/platform_left.png";
        break;
    case PLATFORM_RIGHT:
        texturePath += "/platform_right.png";
        break;
    case GOAL:
        texturePath += "/goal_flag.png";
        break;
    case GOAL_BASE:
        texturePath += "/goal_base.png";
        break;
    case GOAL_MIDDLE:
        texturePath += "/goal_middle.png";
        break;
    case BONUS:
        texturePath += "/bonus_block.png";
        break;
    case FLOOR_COVERED_CORNER_LEFT:
        texturePath += "/floor_covered_corner_left.png";
        break;
    case FLOOR_COVERED_CORNER_RIGHT:
        texturePath += "/floor_covered_corner_right.png";
        break;
    case GAP_BLOCK:
        texturePath += "/empty_block.png";
        break;
    case NO_BLOCK_TYPE:
        texturePath += "/empty_block.png";
        break;
    default:
        texturePath += "/empty_block.png";
        break;
    }

    texture = new QPixmap(texturePath);

    xDim = yDim = 1;

    if(!texture->isNull())
        texture->scaled(blockSize.x, blockSize.y);
    else
        qDebug() << "NULL texture for block type: " << bt << texturePath;
}
