#ifndef NETWORK_H
#define NETWORK_H

#include "neuron.h"
#include "nnglobals.h"
#include <QVector>
#include <QMap>
#include <QString>
#include <cmath>

class Network
{
public:
    Network();
    ~Network();

    QMap<QString, bool> EvaluateNetwork(QVector<int> inputs);

    QMap<int, Neuron*> neurons;

    static float sigmoid(float val);
};

#endif // NETWORK_H
