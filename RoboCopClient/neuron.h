#ifndef NEURON_H
#define NEURON_H

#include "gene.h"
#include <QVector>

class Neuron
{
public:
    Neuron();

private:
    QVector<Gene> incoming;
    float value;
};

#endif // NEURON_H
