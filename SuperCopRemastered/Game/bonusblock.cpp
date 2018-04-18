#include "bonusblock.h"

BonusBlock::BonusBlock()
{
//    srand(time(0));
    this->hitsLeft = 1;
}

BonusBlock::BonusBlock(int maxHits) : BlockBase(NO_LEVEL_TYPE, BONUS)
{
    this->hitsLeft = maxHits;
}

int BonusBlock::GetHitsRemaining()
{
    return this->hitsLeft;
}

void BonusBlock::SetHits(int maxHits)
{
    this->hitsLeft = maxHits;
}

void BonusBlock::BlockHit()
{
//    double res = ((double) rand() / RAND_MAX);

//    if(res > 0.15)
//    {
//        emit ItemGenerated(DONUT);
//    }
//    else
//    {
//        emit ItemGenerated(ONE_UP);
//    }

    this->hitsLeft--;
}
