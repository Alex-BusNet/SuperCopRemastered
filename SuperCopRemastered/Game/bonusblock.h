#ifndef BONUSBLOCK_H
#define BONUSBLOCK_H

#include "blockbase.h"
#include <random>
#include <ctime>

class BonusBlock : public BlockBase
{
public:
    BonusBlock();
    BonusBlock(int maxHits);

    int GetHitsRemaining();

private:
    int hitsLeft;

public slots:
    void BlockHit();

signals:
//    void ItemGenerated(ItemType iType);
};

#endif // BONUSBLOCK_H
