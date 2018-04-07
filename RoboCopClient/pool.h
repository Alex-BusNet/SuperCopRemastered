#ifndef POOL_H
#define POOL_H

#include "species.h"
#include "nnglobals.h"
#include <QVector>

class Pool
{
public:
    Pool();

    int NewInnovation();
    int TotalAverageFitness();

    void RankGlobally();
    void NextGenome();
    void NewGeneration();

    QMap<QString, bool> EvaluateNetwork(QVector<int> inputs);

    void AddToSpecies(Genome *child);
    void CullSpecies(bool cutToOne);
    void RemoveStaleSpecies();
    void RemoveWeakSpecies();

    void SetCurrentFrame(int frame);

    void SetMaxFitness(int mf);
    void SetCurrentSpecies(int cs);
    void SetCurrentGenome(int gm);

    bool FitnessAlreadyMeasured();

    QVector<Species*> species;

    int GetCurrentGenome();
    int GetInnovation();
    int GetGeneration();
    int GetCurrentSpecies();
    int GetCurrentFrame();
    int GetMaxFitness();

private:
    int generation;
    int innovation;
    int currentSpecies;
    int currentGenome;
    int currentFrame;
    int maxFitness;
};

#endif // POOL_H
