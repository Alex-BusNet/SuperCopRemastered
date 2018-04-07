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
    return this->mutationRates["bias"];
}

float Genome::GetConnectionsChance()
{
    return this->mutationRates["connections"];
}

float Genome::GetLinkChance()
{
    return this->mutationRates["link"];
}

float Genome::GetNodeChance()
{
    return this->mutationRates["node"];
}

float Genome::GetEnableChance()
{
    return this->mutationRates["enable"];
}

float Genome::GetDisableChance()
{
    return this->mutationRates["disable"];
}

float Genome::GetStep()
{
    return this->mutationRates["step"];
}

int Genome::GetFitness()
{
    return this->fitness;
}

int Genome::GetAdjustedFitness()
{
    return this->adjustedFitness;
}

int Genome::GetMaxNeuron()
{
    return this->maxNeuron;
}

int Genome::GetGlobalRank()
{
    return this->globalRank;
}

void Genome::SetFitness(int fit)
{
    this->fitness = fit;
}

void Genome::SetAdjustedFitness(int adjFit)
{
    this->adjustedFitness = adjFit;
}

void Genome::SetMaxNeuron(int mn)
{
    this->maxNeuron = mn;
}

void Genome::SetGlobalRank(int gr)
{
    this->globalRank = gr;
}

void Genome::CopyMutationRates(QMap<QString, float> mr)
{
    this->mutationRates = mr;
}

bool Genome::ContainsLink(Gene link)
{
    foreach(Gene *g, genes)
    {
        if(g->into == link.into && g->out == link.out)
            return true;
    }

    return false;
}

bool Genome::SameSpecies(const Genome &other, int innovationSize)
{
    float dd = RoboCop::DeltaDisjoint * Disjoint(other.genes, innovationSize);
    float dw = RoboCop::DeltaWeights * Weights(other.genes, innovationSize);
    return (dd + dw) < RoboCop::DeltaThreshold;
}

int Genome::Disjoint(const QVector<Gene*> &other, int innovationSize)
{
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

int Genome::RandomNeuron(bool nonInput)
{
    QVector<bool> neurons;
    int i = 0;
    if(!nonInput)
    {
        for(i = 0; i < RoboCop::Inputs; i++)
        {
            neurons.push_back(true);
        }
    }

//    std::cout << "Filling temp vect to maxnodes" << std::endl;
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
