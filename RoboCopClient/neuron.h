#ifndef NEURON_H
#define NEURON_H

#include "gene.h"
#include <QMap>

class Neuron
{
public:
    Neuron();

//private:
    QMap<int, Gene*> incoming;
    float value;
};

#endif // NEURON_H
