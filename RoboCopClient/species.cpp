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
    int total = 0;
    foreach(Genome *gm, genomes)
    {
        total += gm->GetGlobalRank();
    }

    averageFitness = total / genomes.size();
}

int Species::GetTopFitness()
{
    return this->topFitness;
}

int Species::GetStaleness()
{
    return this->staleness;
}

int Species::GetAverageFitness()
{
    return this->averageFitness;
}

void Species::SetTopFitness(int fit)
{
    this->topFitness = fit;
}

void Species::SetStaleness(int stale)
{
    this->staleness = stale;
}

Genome *Species::BreedChild(int innovationSize)
{
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
