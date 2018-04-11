#ifndef ROBOCOPHANDLER_H
#define ROBOCOPHANDLER_H

// Used to manage the NN / Genetic Algorithm
#include <QObject>

#include "nnglobals.h"
#include "gene.h"
#include "genome.h"
#include "pool.h"
#include "neuron.h"
#include "network.h"
#include "species.h"

#include <ctime>
#include <cmath>

class RoboCopHandler : public QObject
{
    Q_OBJECT
public:
    RoboCopHandler();
    RoboCopHandler(const RoboCopHandler &rch);
    ~RoboCopHandler();

    void GameLoop();

    void InitializeRun();
    void InitializePool();
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

    Pool *GetPool();
    QVector<int> GetInputs();
    int GetPlayerPosX();
    int GetRightMost();
    int GetTimeout();
    QMap<QString, bool> GetControls();

private:
    Pool *pool;
    QVector<int> inputs;
    int playerPosX;
    int rightmost = 0;
    int timeout;

    QMap<QString, bool>controls;

signals:
    void keyStateUpdate(uint8_t keyState);
};

#endif // ROBOCOPHANDLER_H
