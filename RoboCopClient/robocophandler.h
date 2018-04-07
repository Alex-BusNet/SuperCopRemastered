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

#include <ctime>
#include <cmath>

class RoboCopHandler
{
public:
    RoboCopHandler();

    void GameLoop();

    void InitializeRun();
    void SetInputs(int **in);
    void SetPosition(int x);

    void EvaluateCurrent();
    void RankGlobally();

    void SetControls(QMap<QString, bool> cState);
    void ClearControls();

    void Mutate(Genome *g);
    void PointMutate(Genome *g);
    void LinkMutate(Genome *g, bool bias);
    void NodeMutate(Genome *g);
    void EnableDisableMutate(Genome *g, bool enable);

    Genome *BasicGenome();

private:
    Pool *pool;
    QVector<int> inputs;
    int playerPosX;
    int rightmost = 0;
    int timeout;

    QMap<QString, bool>controls;
};

#endif // ROBOCOPHANDLER_H
