#ifndef GENOME_H
#define GENOME_H

#include "gene.h"
#include "network.h"
#include "nnglobals.h"

#include <QVector>
#include <QMap>
#include <QString>
#include <QMap>
#include <cmath>
#include <ctime>

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
    Genome(Genome &g);

    void GenerateNetwork();

    float GetBiasChance();
    float GetConnectionsChance();
    float GetLinkChance();
    float GetNodeChance();
    float GetEnableChance();
    float GetDisableChance();
    float GetStep();

    int GetFitness();
    int GetAdjustedFitness();
    int GetMaxNeuron();
    int GetGlobalRank();

    void SetFitness(int fit);
    void SetAdjustedFitness(int adjFit);
    void SetMaxNeuron(int mn);
    void SetGlobalRank(int gr);
    void CopyMutationRates(QMap<QString, float> mr);

    bool ContainsLink(Gene link);
    bool SameSpecies(const Genome &other, int innovationSize);
    int Disjoint(const QVector<Gene*> &other, int innovationSize);
    float Weights(const QVector<Gene*> &other, int innovationSize);


    Genome* Crossover(Genome &other, int innovationSize);


    QVector<Gene*> genes;
    Network network;

    int RandomNeuron(bool nonInput);

    QMap<QString, float> mutationRates;

private:

    int fitness;
    int adjustedFitness;
    int maxNeuron;
    int globalRank;
};

#endif // GENOME_H
