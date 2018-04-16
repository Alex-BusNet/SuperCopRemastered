#ifndef SPECIES_H
#define SPECIES_H

#include "genome.h"
#include <QVector>
#include <cmath>
#include <ctime>

class Genome;

class Species
{
public:
    Species();
    ~Species();

    void CalculateAverageFitness();

    float GetTopFitness();
    int GetStaleness();
    float GetAverageFitness();

    void SetTopFitness(int fit);
    void SetStaleness(int stale);

    Genome *BreedChild(int innovationSize);

    QVector<Genome*> genomes;

    void SaveSpecies(QJsonObject &obj);
    void LoadSpecies(const QJsonObject &obj);

private:
    float topFitness;
    int staleness;
    float averageFitness;
};

#endif // SPECIES_H
