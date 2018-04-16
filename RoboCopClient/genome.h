#ifndef GENOME_H
#define GENOME_H

#include "gene.h"
#include "network.h"
#include "nnglobals.h"
#include "robocophandler.h"

#include <QVector>
#include <QMap>
#include <QString>
#include <QMap>
#include <cmath>
#include <ctime>

class Genome
{
public:
    Genome();
    Genome(Genome &g);
    ~Genome();

    static Genome* BasicGenome();

    void GenerateNetwork();

    static void Mutate(Genome *g);
    static void PointMutate(Genome *g);
    static void LinkMutate(Genome *g, bool bias);
    static void NodeMutate(Genome *g);
    static void EnableDisableMutate(Genome *g, bool enable);

    float GetBiasChance();
    float GetConnectionsChance();
    float GetLinkChance();
    float GetNodeChance();
    float GetEnableChance();
    float GetDisableChance();
    float GetStep();

    float GetFitness();
    float GetAdjustedFitness();
    int GetMaxNeuron();
    int GetGlobalRank();

    void SetFitness(float fit);
    void SetAdjustedFitness(float adjFit);
    void SetMaxNeuron(int mn);
    void SetGlobalRank(int gr);
    void CopyMutationRates(QMap<QString, float> mr);

    bool ContainsLink(Gene link);
    bool SameSpecies(const Genome &other, int innovationSize);
    float Disjoint(const QVector<Gene*> &other, int innovationSize);
    float Weights(const QVector<Gene*> &other, int innovationSize);


    Genome* Crossover(Genome *other, int innovationSize);

    void SaveGenome(QJsonObject &obj);
    void LoadGenome(const QJsonObject &obj);

    QVector<Gene*> genes;
    Network *network;

    int RandomNeuron(bool nonInput);

    QMap<QString, float> mutationRates;

private:

    float fitness;
    float adjustedFitness;
    int maxNeuron;
    int globalRank;
};

#endif // GENOME_H
