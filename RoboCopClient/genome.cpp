#include "genome.h"
#include "sorthelper.h"

/**
 * @brief Genome default c'tor
 */
Genome::Genome()
{
    fitness = 0.0f;
    adjustedFitness = 0.0f;
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

    foreach(Gene* gene, g.genes)
    {
        this->genes.push_back(new Gene(*gene));
    }

    this->mutationRates.insert("bias", g.GetBiasChance());
    this->mutationRates.insert("disable", g.GetDisableChance());
    this->mutationRates.insert("enable", g.GetEnableChance());
    this->mutationRates.insert("link", g.GetLinkChance());
    this->mutationRates.insert("connections", g.GetConnectionsChance());
    this->mutationRates.insert("node", g.GetNodeChance());

    this->fitness = 0.0f;
    this->adjustedFitness = 0.0f;
    this->maxNeuron = 0;
    this->globalRank = 0;
}

Genome::~Genome()
{
    foreach(Gene *g, genes)
    {
        if(g != NULL)
            delete g;
    }

    if(network != NULL)
        delete network;
}

Genome *Genome::BasicGenome()
{
    Genome *g = new Genome();
    g->SetMaxNeuron(RoboCop::Inputs);
    Mutate(g);
    return g;
}

bool lowToHighGene(Gene* a, Gene* b) { return a->out < b->out; }
bool highToLowGene(Gene* a, Gene* b) { return a->out > b->out; }

void Genome::GenerateNetwork()
{
//    qDebug() << "GenerateNetwork()";
    network = new Network();

    int i = 0;
    for(; i < RoboCop::Inputs; i++)
    {
        if(network->neurons.contains(i))
            network->neurons[i] = new Neuron();
        else
            this->network->neurons.insert(i, new Neuron());
    }

    for(i = 0; i < RoboCop::Outputs; i++)
    {
        if(network->neurons.contains(RoboCop::MaxNodes + i))
            network->neurons[RoboCop::MaxNodes + i] = new Neuron();
        else
            network->neurons.insert(RoboCop::MaxNodes + i, new Neuron());
    }

    // Sort genes from lowest out to highest out
    RoboCop::Quicksort(genes, 0, genes.size() - 1, lowToHighGene);

//    for(i = 0; i < genes.size(); i++)
//    {
//        for(int j = i+1; j < genes.size(); j++)
//        {
//            if(genes[i]->out > genes[j]->out)
//            {
//                Gene* g = genes[i];
//                genes[i] = genes[j];
//                genes[j] = g;
//            }
//        }
//    }

    foreach(Gene *g, genes)
    {
        if(g->enabled)
        {
            if(!network->neurons.contains(g->out))
            {
                network->neurons.insert(g->out, new Neuron());
            }

            Neuron *neuron = network->neurons[g->out];

            if(!neuron->incoming.contains(g->out))
                neuron->incoming.insert(g->out, g);
            else
                neuron->incoming[g->out] = g;

            if(!network->neurons.contains(g->into))
                network->neurons.insert(g->into, new Neuron());
        }
    }
}

void Genome::Mutate(Genome *g)
{
//    qDebug() << "Mutate()";
    foreach(QString s, g->mutationRates.keys())
    {
        if((rand() % 2) == 1)
            g->mutationRates[s] *= 0.95f; // Values from MarI/O
        else
            g->mutationRates[s] *= 1.05263f; // Values from MarI/O
    }

    float p = g->mutationRates["connections"];

    if(RoboCop::randomf() < p)
        PointMutate(g);

    p = g->mutationRates["link"];
    while (p > 0)
    {
        if (RoboCop::randomf() < p)
            LinkMutate(g, false);

        p -= 1;
    }

    p = g->mutationRates["bias"];
    while(p > 0)
    {
        if (RoboCop::randomf() < p)
            LinkMutate(g, true);

        p -= 1;
    }

    p = g->mutationRates["node"];
    while(p > 0)
    {
        if (RoboCop::randomf() < p)
            NodeMutate(g);

        p -= 1;
    }

    p = g->mutationRates["enable"];
    while(p > 0)
    {
        if (RoboCop::randomf() < p)
            EnableDisableMutate(g, true);

        p -= 1;
    }

    p = g->mutationRates["disable"];
    while(p > 0)
    {
        if (RoboCop::randomf() < p)
            EnableDisableMutate(g, false);

        p -= 1;
    }
}

void Genome::PointMutate(Genome *g)
{
    //    qDebug() << "PointMutate()";
    float step = g->GetStep();

    for(int i = 0; i < g->genes.size(); i++)
    {
        Gene *gene = g->genes[i];
        float p = RoboCop::randomf();
        if(p < RoboCop::PerturbChance)
            gene->weight += (RoboCop::randomf() * step * 2) - step;
        else
            gene->weight = RoboCop::randomf() * 4 - 2;
    }
}

void Genome::LinkMutate(Genome *g, bool bias)
{
    //    qDebug() << "LinkMutate()";
    int neuron1 = g->RandomNeuron(false);
    int neuron2 = g->RandomNeuron(true);

    Gene *l = new Gene();
    // Check if both selected neurons are inputs.
    if(neuron1 <= RoboCop::Inputs && neuron2 <= RoboCop::Inputs)
        return;

    if(neuron2 <= RoboCop::Inputs)
    {
        // Swap Input and Output
        // (We want neuron2 to be the output neuron)
        int temp = neuron1;
        neuron1 = neuron2;
        neuron2 = temp;
    }

    l->into = neuron1;
    l->out = neuron2;

    if(bias) { l->into = RoboCop::Inputs; }

    if(g->ContainsLink(*l)) { return; }

    l->innovation = RoboCopHandler::instance()->GetPool()->NewInnovation() - 1;
    l->weight = RoboCop::randomf() * 4.0f - 2.0f;

    g->genes.push_back(l);
}

void Genome::NodeMutate(Genome *gm)
{
//    qDebug() << "NodeMutate()";
    if(gm->genes.size() == 0) { return; }

    gm->SetMaxNeuron(gm->GetMaxNeuron() + 1);

    int idx = RoboCop::randomi() % gm->genes.size();
    Gene *gene = gm->genes[idx];
    if(!gene->enabled) { return; }

    gene->enabled = false;

    Gene *g1 = new Gene(*gene);
    g1->out = gm->GetMaxNeuron();
    g1->weight = 1.0;

    // The innovation is treated as an index,
    // therefore we want to subtract 1 from the
    // innovation to keep the value within addressable
    // bounds when needed.
    g1->innovation = RoboCopHandler::instance()->GetPool()->NewInnovation() - 1;
    g1->enabled = true;
    gm->genes.push_back(g1);

    Gene *g2 = new Gene(*gene);
    g2->into = gm->GetMaxNeuron();
    g2->innovation = RoboCopHandler::instance()->GetPool()->NewInnovation() - 1;
    g2->enabled = true;
    gm->genes.push_back(g2);
}

void Genome::EnableDisableMutate(Genome *gm, bool enable)
{
//    qDebug() << "EnableDisableMutate()";
    QVector<Gene*> candidates;

    foreach(Gene *gene, gm->genes)
    {
        if(gene->enabled == !enable)
            candidates.push_back(gene);
    }

    if(candidates.size() == 0)
        return;

    Gene *g1 = candidates[rand() % candidates.size()];
    g1->enabled = !g1->enabled;
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

float Genome::GetFitness()
{
//    qDebug() << "GetFitness()";
    return this->fitness;
}

float Genome::GetAdjustedFitness()
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

void Genome::SetFitness(float fit)
{
//    qDebug() << "SetFitness()";
    this->fitness = fit;
}

void Genome::SetAdjustedFitness(float adjFit)
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
    foreach(QString s, this->mutationRates.keys())
    {
        this->mutationRates[s] = mr[s];
    }
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
//    qDebug() << "dd: " << dd << " dw: " << dw << (dd + dw);
    return (dd + dw) < RoboCop::DeltaThreshold;
}

float Genome::Disjoint(const QVector<Gene*> &other, int innovationSize)
{
//    qDebug() << "Disjoint()";
    QMap<int, bool> i1;
    QMap<int, bool> i2;

    foreach(Gene *g1, genes)
    {
        i1.insert(g1->innovation, true);
    }

    foreach(Gene *g2, other)
    {
        i2.insert(g2->innovation, true);
    }

    int disjointGenes = 0;

    foreach(Gene *g1, genes)
    {
        if(!i2.contains(g1->innovation))
            disjointGenes++;
    }

    foreach(Gene *g2, other)
    {
        if(!i1.contains(g2->innovation))
            disjointGenes++;
    }

    int n = std::max(genes.size(), other.size());
//    qDebug() << "\tDisjoint genes: " << disjointGenes << " N: " << n;
    return (float)disjointGenes / (float)n;
}

float Genome::Weights(const QVector<Gene*> &other, int innovationSize)
{
//    qDebug() << "Weights()";
    QMap<int, Gene*> i2;

    foreach(Gene *g, other)
    {
        i2.insert(g->innovation, g);
    }

    float sum = 0.0f;
    float coincident = 0.0f;

    foreach(Gene *g, genes)
    {
        if(i2.contains(g->innovation))
        {
            Gene *gene2 = i2[g->innovation];
            sum += std::abs(g->weight - gene2->weight);
            coincident++;
        }
    }
//    qDebug() << "\tSum: " << sum << " Coincident: " << coincident;
    return sum / coincident;
}

Genome* Genome::Crossover(Genome *other, int innovationSize)
{
//    qDebug() << "Crossover()\n\tGlobal rank 1: " << this->globalRank << " Global rank 2: " << other.globalRank;
    if(other->GetFitness() > this->fitness)
    {
        return other->Crossover(this, innovationSize);
    }

    Genome* child = new Genome();

    QMap<int, Gene*> innovations2;

    foreach(Gene *g, other->genes)
    {
        if(!innovations2.contains(g->innovation))
            innovations2.insert(g->innovation, g);
        else
            innovations2[g->innovation] = g;
    }

    for(int i = 0; i < this->genes.size(); i++)
    {
        Gene* g1 = this->genes[i];
        Gene* g2 = innovations2.contains(g1->innovation) ? innovations2[g1->innovation] : NULL;

        int p = RoboCop::randomi() % 2;

        if(g2 != NULL && p == 0 && g2->enabled)
            child->genes.push_back(new Gene(*g2));
        else
            child->genes.push_back(new Gene(*g1));
    }

    child->SetMaxNeuron(std::max(this->maxNeuron, other->GetMaxNeuron()));

    child->CopyMutationRates(this->mutationRates);
//    qDebug() << "--Finished Crossover()";
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
    QMap<int, bool> neurons;
    int i = 0;

    // Create an (int, bool) pair in the neurons QMap for
    // every input node (if nonInput = false) and every
    // output node
    if(!nonInput)
    {
        for(i = 0; i < RoboCop::Inputs; i++)
        {
            if(!neurons.contains(i))
                neurons.insert(i, true);
            else
                neurons[i] = true;
        }
    }

    for(i = 0; i < RoboCop::Outputs; i++)
    {
        if(!neurons.contains(RoboCop::MaxNodes + i))
            neurons.insert(RoboCop::MaxNodes + i, true);
        else
            neurons[RoboCop::MaxNodes + i] = true;
    }


    for(i = 0; i < this->genes.size(); i++)
    {
        if(!nonInput || (genes[i]->into > RoboCop::Inputs))
        {
            neurons[genes[i]->into] = true;
        }

        if(!nonInput || (genes[i]->out > RoboCop::Inputs))
        {
            neurons[genes[i]->out] = true;
        }
    }

    int n = RoboCop::randomi() % neurons.size();
    foreach(int k, neurons.keys())
    {
        n -= 1;
        if( n == 0 )
        {
            return k;
        }
    }

    return 0;
}
