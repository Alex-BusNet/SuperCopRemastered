#include "network.h"

Network::Network()
{

}

Network::~Network()
{
    foreach(int k, neurons.keys())
    {
        if(neurons[k] != NULL)
            delete neurons[k];
    }
}

QMap<QString, bool> Network::EvaluateNetwork(QVector<int> inputs)
{
//    qDebug() << "EvaluateNetwork()";
    QMap<QString, bool> out;
    int i;
    for(i = 0; i < RoboCop::Outputs; i++)
    {
        out.insert(RoboCop::ButtonNames[i], false);
    }

    inputs.push_back(1);

    if(inputs.size() != RoboCop::Inputs)
    {
        qDebug() << "Invalid Input size";
        return out;
    }

//    qDebug() << "Loading inputs...";
    for(i = 0; i < RoboCop::Inputs; i++)
    {
        if(!neurons.contains(i))
            neurons.insert(i, new Neuron());

        this->neurons[i]->value = inputs[i];
    }

//    qDebug() << "Evaluating neurons...";
    foreach(int k, neurons.keys())
    {
        Neuron* n = neurons[k];

        if(n != NULL)
        {
            float sum = 0;

//            if(n->incoming.size() > 1)
//                qDebug() << "\t(" << k << ") Incoming size: " << n->incoming.size();

            foreach(int j, n->incoming.keys())
            {
                Gene *inc = n->incoming[j];
                if(inc != NULL)
                {
                    if(neurons.contains(inc->into))
                    {
                        Neuron *oth = this->neurons[inc->into];

                        if(oth != NULL)
                        {
                            sum += inc->weight * oth->value;
                        }
                    }
                }
            }

            if(n->incoming.size() > 0)
                n->value = sigmoid(sum);
        }
    }

//    qDebug() << "Analyzing outputs...";
    for(i = 0; i < RoboCop::Outputs; i++)
    {
        if(neurons.contains(RoboCop::MaxNodes + i))
        {
            if(this->neurons[RoboCop::MaxNodes + i]->value > 0)
            {
//                qDebug() << "Setting" << RoboCop::ButtonNames[i];
                out[RoboCop::ButtonNames[i]] = true;
            }
            else
                out[RoboCop::ButtonNames[i]] = false;
        }
    }
//    qDebug() << "--Finished EvaluateNetwork()";
    return out;
}

float Network::sigmoid(float val)
{
//    qDebug() << "Sigmoid()";
    return (2.0f / (1.0f + std::exp2f(-4.9 * val))) - 1;
}
