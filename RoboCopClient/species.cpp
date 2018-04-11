#include "species.h"

Species::Species()
{
    topFitness = 0;
    staleness = 0;
    averageFitness = 0;

    srand(time(0));
}

void Species::CalculateAverageFitness()
{
//    qDebug() << "CalculateAverageFitness()";
    int total = 0;
    foreach(Genome *gm, genomes)
    {
        total += gm->GetGlobalRank();
    }

    averageFitness = total / genomes.size();
}

int Species::GetTopFitness()
{
//    qDebug() << "GetTopFitness()";
    return this->topFitness;
}

int Species::GetStaleness()
{
//    qDebug() << "GetStaleness()";
    return this->staleness;
}

int Species::GetAverageFitness()
{
//    qDebug() << "GetAverageFitness()";
    return this->averageFitness;
}

void Species::SetTopFitness(int fit)
{
//    qDebug() << "SetTopFitness()";
    this->topFitness = fit;
}

void Species::SetStaleness(int stale)
{
//    qDebug() << "SetStaleness()";
    this->staleness = stale;
}

Genome *Species::BreedChild(int innovationSize)
{
//    qDebug() << "BreedChild()";

    Genome *child;
    float p = ((float)rand() / (float)RAND_MAX);
    if(p < RoboCop::CrossoverChance)
    {
        Genome *g1 = genomes[rand() % genomes.size()];
        Genome *g2 = genomes[rand() % genomes.size()];

        child = g1->Crossover(*g2, innovationSize);
    }
    else
    {
        Genome *g = genomes[rand() % genomes.size()];
        child = new Genome(*g);
    }

    return child;
}

void Species::SaveSpecies(QJsonObject &obj)
{
    obj["topfitness"] = topFitness;
    obj["staleness"] = staleness;
    QJsonArray arr;
    foreach(Genome *g, genomes)
    {
        QJsonObject gObj;
        g->SaveGenome(gObj);
        arr.push_back(gObj);
    }

    obj["genomes"] = arr;
}

void Species::LoadSpecies(const QJsonObject &obj)
{
    topFitness = obj["topfitness"].toInt();
    staleness = obj["staleness"].toInt();

    QJsonArray arr = obj["genomes"].toArray();

    for(int i = 0; i < arr.size(); i++)
    {
        QJsonObject gObj = arr.at(i).toObject();
        Genome *g = new Genome();
        g->LoadGenome(gObj);
        genomes.push_back(g);
    }
}
