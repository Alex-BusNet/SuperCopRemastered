#ifndef SPECIES_H
#define SPECIES_H

#include "genome.h"
#include <QVector>

class Species
{
public:
    Species();

private:
    int topFitness;
    int staleness;
    QVector<Genome> genomes;
    int averageFitness;
};

#endif // SPECIES_H
