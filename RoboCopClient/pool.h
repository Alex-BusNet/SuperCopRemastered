#ifndef POOL_H
#define POOL_H

#include "species.h"
#include "nnglobals.h"
#include <QVector>

class Genome;
class Species;

class Pool
{
public:
    Pool();
    ~Pool();

    int NewInnovation();
    float TotalAverageFitness();

    void RankGlobally();
    void NextGenome();
    void NewGeneration();

    QMap<QString, bool> EvaluateNetwork(QVector<int> inputs);

    void AddToSpecies(Genome *child);
    void CullSpecies(bool cutToOne);
    void RemoveStaleSpecies();
    void RemoveWeakSpecies();

    void SetCurrentFrame(int frame);

    void SetMaxFitness(int mf);
    void SetCurrentSpecies(int cs);
    void SetCurrentGenome(int gm);

    bool FitnessAlreadyMeasured();

    QVector<Species*> species;

    int GetCurrentGenome();
    int GetInnovation();
    int GetGeneration();
    int GetCurrentSpecies();
    int GetCurrentFrame();
    int GetMaxFitness();

    void SavePool(QJsonObject &obj);
    void LoadPool(QJsonObject &obj);

private:
    int generation;
    int innovation;
    int currentSpecies;
    int currentGenome;
    int currentFrame;
    int maxFitness;

    void SaveFile(QString filename);
    void LoadFile(QString filename);
};

#endif // POOL_H
