#include "genome.h"

/**
 * @brief Genome default c'tor
 */
Genome::Genome()
{
    fitness = 0;
    adjustedFitness = 0;
    maxNeuron = 0;
    globalRank = 0;
    mutationRates.insert("bias", RoboCop::BiasMutationChance);
    mutationRates.insert("disable", RoboCop::DisableMutationChance);
    mutationRates.insert("enable", RoboCop::EnableMutationChance);
    mutationRates.insert("link", RoboCop::LinkMutationChance);
    mutationRates.insert("connections", RoboCop::MutationConnectionChance);
    mutationRates.insert("node", RoboCop::NodeMutationChance);
    mutationRates.insert("step", RoboCop::StepSize);
}

/**
 * @brief Copies Genome, g, to a new Genome
 * @param g: The Genome to be copied
 */
Genome::Genome(Genome &g) : Genome()
{
    this->genes = g.genes;
    this->maxNeuron = g.maxNeuron;
    this->mutationRates.insert("bias", g.GetBiasChance());
    this->mutationRates.insert("disable", RoboCop::DisableMutationChance);
    this->mutationRates.insert("enable", RoboCop::EnableMutationChance);
    this->mutationRates.insert("link", RoboCop::LinkMutationChance);
    this->mutationRates.insert("connections", RoboCop::MutationConnectionChance);
    this->mutationRates.insert("node", RoboCop::NodeMutationChance);
    this->fitness = 0;
    this->adjustedFitness = 0;
    this->maxNeuron = 0;
    this->globalRank = 0;
}

void Genome::GenerateNetwork()
{
//    qDebug() << "GenerateNetwork()";
    int i = 0;
    for(; i < RoboCop::Inputs; i++)
    {
        this->network.neurons.push_back(new Neuron());
    }

    // Fill in the neuron vector with blank nodes up to MaxNodes.
    for(; i < RoboCop::MaxNodes; i++)
    {
        this->network.neurons.push_back(NULL);
    }

    // Sort genes
    for(i = 0; i < genes.size(); i++)
    {
        for(int j = i+1; j < genes.size(); j++)
        {
            if(genes[i]->out > genes[j]->out)
            {
                Gene* g = genes[i];
                genes[i] = genes[j];
                genes[j] = g;
            }
        }
    }

    foreach(Gene *g, genes)
    {
        if(g->enabled)
        {
            if(network.neurons[g->out] == NULL)
            {
                network.neurons[g->out] = new Neuron();
            }

            Neuron *neuron = network.neurons[g->out];
            neuron->incoming.push_back(g);
            if(network.neurons[g->into] == NULL)
                network.neurons[g->into] = new Neuron();
        }
    }
}


float Genome::GetBiasChance()
{
//    qDebug() << "GetBiasChance()";
    return this->mutationRates["bias"];
}

float Genome::GetConnectionsChance()
{
//    qDebug() << "GetConnectionsChance()";
    return this->mutationRates["connections"];
}

float Genome::GetLinkChance()
{
//    qDebug() << "GetLinkChance()";
    return this->mutationRates["link"];
}

float Genome::GetNodeChance()
{
//    qDebug() << "GetNodeChance()";
    return this->mutationRates["node"];
}

float Genome::GetEnableChance()
{
//    qDebug() << "GetEnableChance()";
    return this->mutationRates["enable"];
}

float Genome::GetDisableChance()
{
//    qDebug() << "GetDisableChance()";
    return this->mutationRates["disable"];
}

float Genome::GetStep()
{
//    qDebug() << "GetStep()";
    return this->mutationRates["step"];
}

int Genome::GetFitness()
{
//    qDebug() << "GetFitness()";
    return this->fitness;
}

int Genome::GetAdjustedFitness()
{
//    qDebug() << "GetAdjustedFitness()";
    return this->adjustedFitness;
}

int Genome::GetMaxNeuron()
{
//    qDebug() << "GetMaxNeuron()";
    return this->maxNeuron;
}

int Genome::GetGlobalRank()
{
//    qDebug() << "GetGlobalRank()";
    return this->globalRank;
}

void Genome::SetFitness(int fit)
{
//    qDebug() << "SetFitness()";
    this->fitness = fit;
}

void Genome::SetAdjustedFitness(int adjFit)
{
//    qDebug() << "SetAdjustedFitness()";
    this->adjustedFitness = adjFit;
}

void Genome::SetMaxNeuron(int mn)
{
//    qDebug() << "SetMaxNeuron()";
    this->maxNeuron = mn;
}

void Genome::SetGlobalRank(int gr)
{
//    qDebug() << "SetGlobalRank()";
    this->globalRank = gr;
}

void Genome::CopyMutationRates(QMap<QString, float> mr)
{
//    qDebug() << "CopyMutationRates()";
    this->mutationRates = mr;
}

bool Genome::ContainsLink(Gene link)
{
//    qDebug() << "ContainsLink()";
    foreach(Gene *g, genes)
    {
        if(g->into == link.into && g->out == link.out)
            return true;
    }

    return false;
}

bool Genome::SameSpecies(const Genome &other, int innovationSize)
{
//    qDebug() << "SameSpecies()";
    float dd = RoboCop::DeltaDisjoint * Disjoint(other.genes, innovationSize);
    float dw = RoboCop::DeltaWeights * Weights(other.genes, innovationSize);
    return (dd + dw) < RoboCop::DeltaThreshold;
}

int Genome::Disjoint(const QVector<Gene*> &other, int innovationSize)
{
//    qDebug() << "Disjoint()";
    bool i1[innovationSize];
    bool i2[innovationSize];

    for(int i = 0; i < innovationSize; i++)
    {
        i1[i] = false;
        i2[i] = false;
    }

    foreach(Gene *g, genes)
    {
        i1[g->innovation] = true;
    }

    foreach(Gene *g, other)
    {
        i2[g->innovation] = true;
    }

    int disjointGenes = 0;

    foreach(Gene *g, genes)
    {
        if(!i2[g->innovation])
            disjointGenes++;
    }

    foreach(Gene *g, other)
    {
        if(!i1[g->innovation])
            disjointGenes++;
    }

    int n = std::max(genes.size(), other.size());

    return disjointGenes / n;
}

float Genome::Weights(const QVector<Gene*> &other, int innovationSize)
{
//    qDebug() << "Weights()";
    Gene *i2[innovationSize];

    for(int i = 0; i < innovationSize; i++)
    {
        i2[i] = NULL;
    }

    foreach(Gene *g, other)
    {
        i2[g->innovation] = g;
    }

    float sum = 0.0f;
    int coincident = 0;

    foreach(Gene *g, genes)
    {
        if(i2[g->innovation] != NULL)
        {
            Gene *gene2 = i2[g->innovation];
            sum += std::abs(g->weight - gene2->weight);
            coincident++;
        }
    }

    return sum / coincident;
}

Genome* Genome::Crossover(Genome &other, int innovationSize)
{
//    qDebug() << "Crossover()";
    if(other.GetFitness() > this->fitness)
    {
        return other.Crossover(*this, innovationSize);
    }

    Genome* child;
    QVector<Gene*> innovations2;
    innovations2.fill(NULL, innovationSize);

    foreach(Gene *g, other.genes)
    {
        innovations2[g->innovation] = g;
    }

    for(int i = 0; i < this->genes.size(); i++)
    {
        Gene* g1 = this->genes[i];
        Gene* g2 = innovations2[g1->innovation];
        int p = rand() % 3;

        if(g2 != NULL && p == 1 && g2->enabled)
            child->genes.push_back(new Gene(*g2));
        else
            child->genes.push_back(new Gene(*g1));
    }

    child->SetMaxNeuron(std::max(this->maxNeuron, other.GetMaxNeuron()));

    child->CopyMutationRates(this->mutationRates);

    return child;
}

void Genome::SaveGenome(QJsonObject &obj)
{
    obj["fitness"] = fitness;
    obj["maxneuron"] = maxNeuron;
    obj["bias"] = mutationRates["bias"];
    obj["disable"] = mutationRates["disable"];
    obj["enable"] = mutationRates["enable"];
    obj["link"] = mutationRates["link"];
    obj["node"] = mutationRates["node"];
    obj["step"] = mutationRates["step"];

    QJsonArray arr;
    foreach(Gene *g, genes)
    {
        QJsonObject gObj;
        g->SaveGene(gObj);
        arr.push_back(gObj);
    }

    obj["genes"] = arr;
}

void Genome::LoadGenome(const QJsonObject &obj)
{
    fitness = obj["fitness"].toInt();
    maxNeuron = obj["maxneuron"].toInt();
    mutationRates["bias"] = (float)obj["bias"].toDouble();
    mutationRates["disable"] = (float)obj["disable"].toDouble();
    mutationRates["enable"] = (float)obj["enable"].toDouble();
    mutationRates["link"] = (float)obj["link"].toDouble();
    mutationRates["node"] = (float)obj["node"].toDouble();
    mutationRates["step"] = (float)obj["step"].toDouble();

    QJsonArray arr = obj["genes"].toArray();
    for(int i = 0; i < arr.size(); i++)
    {
        QJsonObject gObj = arr.at(i).toObject();
        Gene *g = new Gene();
        g->LoadGene(gObj);
        arr.push_back(gObj);
    }
}

int Genome::RandomNeuron(bool nonInput)
{
//    qDebug() << "RandomNeuron()";
    QVector<bool> neurons;
    int i = 0;
    if(!nonInput)
    {
        for(i = 0; i < RoboCop::Inputs; i++)
        {
            neurons.push_back(true);
        }
    }

    for(; i < RoboCop::MaxNodes; i++)
    {
        neurons.push_back(false);
    }

    for(i = 0; i < RoboCop::Outputs; i++)
    {
        neurons.push_back(true);
    }

    for(i = 0; i < this->genes.size(); i++)
    {
        if(!nonInput || genes[i]->into > RoboCop::Inputs)
        {
            neurons[genes[i]->into] = true;
        }

        if(!nonInput || genes[i]->out > RoboCop::Inputs)
        {
            neurons[genes[i]->out] = true;
        }
    }

    int n = rand() % neurons.size();
    for(i = 0; i < neurons.size(); i++)
    {
        n -= 1;
        if( n == 0 ) { return i; }
    }

    return 0;
}
