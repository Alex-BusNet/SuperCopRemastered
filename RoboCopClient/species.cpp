#include "species.h"

Species::Species()
{
//    qDebug() << "Species default c'tor";
    topFitness = 0.0f;
    staleness = 0;
    averageFitness = 0.0f;
//    qDebug() << "--Finished Species default c'tor";
}

Species::~Species()
{
    foreach(Genome *gm, genomes)
    {
        if(gm != NULL)
            delete gm;
    }
}

void Species::CalculateAverageFitness()
{
//    qDebug() << "CalculateAverageFitness()";
    float total = 0.0f;
    foreach(Genome *gm, genomes)
    {
        total += (float)gm->GetGlobalRank();
    }

    averageFitness = total / (float)genomes.size();
//    qDebug() << "--Finished CalculateAverageFitness()";
}

float Species::GetTopFitness()
{
//    qDebug() << "GetTopFitness()";
    return this->topFitness;
}

int Species::GetStaleness()
{
//    qDebug() << "GetStaleness()";
    return this->staleness;
}

float Species::GetAverageFitness()
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
    Genome *child = NULL;
    float p = RoboCop::randomf();
    if(p < RoboCop::CrossoverChance)
    {
//        Genome *g1 = genomes[rand() % genomes.size()];
        Genome *g1 = genomes[0];
        Genome *g2 = genomes[rand() % genomes.size()];

        child = Genome::Crossover(g1, g2, innovationSize);
    }
    else
    {
        Genome *g = genomes[rand() % genomes.size()];
        child = new Genome(*g);
    }

    Genome::Mutate(child);
//    qDebug() << "--Finished BreedChild()";
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
