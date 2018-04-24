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
#include <QFileDialog>

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
    void ResetNN();

    void SetInputs(int **in);
    void SetPositionX(int x);
    void SetPositionY(int y);

    void EvaluateCurrent();
    void RankGlobally();

    void SetControls(QMap<QString, bool> cState);
    void ClearControls();

    bool isGameRunning();

    Pool *GetPool();
    QVector<int> GetInputs();
    int GetPlayerPosX();
    int GetPlayerPosY();
    int GetRightMost();
    int GetTimeout();
    QMap<QString, bool> GetControls();

    int end(int retcode = 0);

    void SaveFile(QString filename);
    void LoadFile(QString filename);

protected:
    void run();

private:
    Pool *pool;
    QVector<int> inputs;
    int playerPosX;
    int playerPosY;
    int prevY;
    bool hasJumped;
    int rightmost = 0, heightMax;
    float timeout;

    bool frameUpdate, gameRunning, reset;

    QString filepath;

    QMap<QString, bool>controls;


signals:
    void keyStateUpdate(uint8_t keyState);
    void GenomeUpdate(int num);
    void SpeciesUpdate(int num, int size);
    void GenerationUpdate(int num);
    void FitnessUpdate(int num);
    void MaxFitnessUpdate(int num);
    void NewSpecies();
    void timeoutval(float num);
    void resetStat(bool stat);

public slots:
    void FrameUpdated();
};

#endif // ROBOCOPHANDLER_H
