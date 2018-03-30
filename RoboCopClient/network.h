#ifndef NETWORK_H
#define NETWORK_H

#include "neuron.h"
#include <QVector>

class Network
{
public:
    Network();

private:
    QVector<Neuron> neurons;
};

#endif // NETWORK_H
