#include "network.h"

Network::Network()
{

}

QMap<QString, bool> Network::EvaluateNetwork(QVector<int> inputs)
{
    QMap<QString, bool> out;
    for(int i = 0; i < RoboCop::Outputs; i++)
    {
        out.insert(RoboCop::ButtonNames[i], false);
    }

    inputs.push_back(1);

    if(inputs.size() != RoboCop::Inputs)
    {
        return out;
    }

    int i = 0;
    for(i = 0; i < RoboCop::Inputs; i++)
    {
        this->neurons[i]->value = inputs[i];
    }

    foreach(Neuron *n, this->neurons)
    {
        float sum = 0;
        for(int j = 0; j < n->incoming.size(); j++)
        {
            Gene *inc = n->incoming[j];
            Neuron *oth = this->neurons[inc->into];
            sum += inc->weight * oth->value;
        }

        if(n->incoming.size() > 0)
            n->value = sigmoid(sum);
    }

    for(; i < RoboCop::MaxNodes; i++)
    {
        neurons.push_back(false);
    }

    for(i = 0; i < RoboCop::Outputs; i++)
    {
        if(this->neurons[RoboCop::MaxNodes + i]->value > 0)
            out[RoboCop::ButtonNames[i]] = true;
        else
            out[RoboCop::ButtonNames[i]] = false;
    }

    return out;
}

float Network::sigmoid(float val)
{
    return 2.0f / (1.0f + std::exp2f(-val));
}
