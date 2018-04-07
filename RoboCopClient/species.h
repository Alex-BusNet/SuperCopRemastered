#ifndef SPECIES_H
#define SPECIES_H

#include "genome.h"
#include <QVector>
#include <cmath>
#include <ctime>

class Species
{
public:
    Species();

    void CalculateAverageFitness();

    int GetTopFitness();
    int GetStaleness();
    int GetAverageFitness();

    void SetTopFitness(int fit);
    void SetStaleness(int stale);

    Genome *BreedChild(int innovationSize);

    QVector<Genome*> genomes;

private:
    int topFitness;
    int staleness;
    int averageFitness;
};

#endif // SPECIES_H
