#ifndef GENOME_H
#define GENOME_H

#include "gene.h"
#include "network.h"
#include "nnglobals.h"

#include <QVector>
#include <QMap>
#include <QString>

typedef struct MutationRates
{
    float MutateConnectionsChance;
    float LinkMutationChance;
    float BiasMutationChance;
    float NodeMutationChance;
    float EnableMutationChance;
    float DisableMutationChance;
    float StepSize;
} MutationRates;

class Genome
{
public:
    Genome();

private:
    QVector<Gene> genes;
    int fitness;
    int adjustedFitness;
    QVector<Network> network;
    int maxNeuron;
    int globalRank;
    MutationRates mutationRates;
};

#endif // GENOME_H
