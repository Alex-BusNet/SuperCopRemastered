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
#include <QThread>
#include <ctime>
#include <cmath>

class Pool;

class RoboCopHandler : public QThread
{
    Q_OBJECT
public:
    RoboCopHandler();
    RoboCopHandler(const RoboCopHandler &rch);
    ~RoboCopHandler();

    static RoboCopHandler *instance();

    void GameLoop();

    void InitializeRun(bool playerDied);
    void InitializePool();

    void LevelReset();

    void SetInputs(int **in);
    void SetPosition(int x);

    void EvaluateCurrent();
    void RankGlobally();

    void SetControls(QMap<QString, bool> cState);
    void ClearControls();

    Pool *GetPool();
    QVector<int> GetInputs();
    int GetPlayerPosX();
    int GetRightMost();
    int GetTimeout();
    QMap<QString, bool> GetControls();

    int end(int retcode = 0);

protected:
    void run();

private:
    Pool *pool;
    QVector<int> inputs;
    int playerPosX;
    int rightmost = 0;
    float timeout;

    bool frameUpdate, gameRunning, reset;

    QString filepath;

    QMap<QString, bool>controls;

    void SaveFile(QString filename);
    void LoadFile(QString filename);

signals:
    void keyStateUpdate(uint8_t keyState);
    void GenomeUpdate(int num);
    void SpeciesUpdate(int num, int size);
    void GenerationUpdate(int num);
    void FitnessUpdate(int num);
    void MaxFitnessUpdate(int num);
    void NewSpecies();

public slots:
    void FrameUpdated();
};

#endif // ROBOCOPHANDLER_H
