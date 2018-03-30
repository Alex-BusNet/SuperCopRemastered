#ifndef POOL_H
#define POOL_H

#include "species.h"
#include "nnglobals.h"
#include <QVector>

class Pool
{
public:
    Pool();

private:
    QVector<Species> species;
    int generation;
    int innovation;
    int currentSpecies;
    int currentGenome;
    int currentFrame;
    int maxFitness;
};

#endif // POOL_H
