#ifndef ROBOCOPHANDLER_H
#define ROBOCOPHANDLER_H

// Used to manage the NN / Genetic Algorithm
#include "nnglobals.h"
#include "gene.h"
#include "genome.h"
#include "pool.h"
#include "neuron.h"
#include "network.h"
#include "species.h"

class RoboCopHandler
{
public:
    RoboCopHandler();

    void GameLoop();
private:
    Pool *pool;


};

#endif // ROBOCOPHANDLER_H
