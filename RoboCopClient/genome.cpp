#include "genome.h"
#include "sorthelper.h"

/**
 * @brief Genome default c'tor
 */
Genome::Genome()
{
//    qDebug() << "Genome default c'tor";
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

//    qDebug() << "--Finished Genome default c'tor";
}

/**
 * @brief Copies Genome, g, to a new Genome
 * @param g: The Genome to be copied
 */
Genome::Genome(Genome &g) : Genome()
{
//    qDebug() << "Genome Copy c'tor";
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
//    qDebug() << "--Finished Genome Copy c'tor";
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
//    qDebug() << "BasicGenome()";
    Genome *g = new Genome();
    g->SetMaxNeuron(RoboCop::Inputs);
    Mutate(g);
//    qDebug() << "--Finished BasicGenome()";
    return g;
}

bool lowToHighGene(Gene* a, Gene* b) { return a->out < b->out; }
bool highToLowGene(Gene* a, Gene* b) { return a->out > b->out; }

void Genome::GenerateNetwork()
{
//    qDebug() << "GenerateNetwork()";
    Network *net = new Network();

    int i = 0;
    for(; i < RoboCop::Inputs; i++)
    {
        net->neurons[i] = new Neuron();
    }

    for(i = 0; i < RoboCop::Outputs; i++)
    {
        net->neurons[RoboCop::MaxNodes + i] = new Neuron();
    }
    // Sort genes from lowest out to highest out
    RoboCop::Quicksort(genes, 0, genes.size() - 1, highToLowGene);

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
            if(!net->neurons.contains(g->out))
            {
                net->neurons.insert(g->out, new Neuron());
            }

            Neuron *neuron = net->neurons[g->out];
            neuron->incoming[g->out] = g;

            if(!net->neurons.contains(g->into))
            {
                net->neurons.insert(g->into, new Neuron());
            }
        }
    }

    this->network = net;
//    qDebug() << "--Finished GenerateNetwork()";
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

//    qDebug() << "--Finished Mutate()";
}

void Genome::PointMutate(Genome *g)
{
//        qDebug() << "PointMutate()";
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

//    qDebug() << "--Finished PointMutate()";
}

void Genome::LinkMutate(Genome *g, bool bias)
{
//    qDebug() << "LinkMutate()";
    int neuron1 = g->RandomNeuron(false);
    int neuron2 = g->RandomNeuron(true);

    // Check if both selected neurons are inputs.
    if(neuron1 <= RoboCop::Inputs && neuron2 <= RoboCop::Inputs)
        return;

    Gene *l = new Gene();

    if(neuron2 <= RoboCop::Inputs)
    {
        // Swap Input and Output
        // (We want neuron2 to be the output neuron)
        int temp = neuron1;
        neuron1 = neuron2;
        neuron2 = temp;
    }

    l->out = neuron2;

    if(bias) { l->into = RoboCop::Inputs; }
    else { l->into = neuron1; }

    if(g->ContainsLink(*l)) { delete l; return; }

    l->innovation = RoboCopHandler::instance()->GetPool()->NewInnovation() - 1;
    l->weight = RoboCop::randomf() * 4.0f - 2.0f;

    g->genes.push_back(l);
//    qDebug() << "--Finished LinkMutate()";
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
    g1->weight = 1.0f;

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

//    qDebug() << "--Finished NodeMutate()";
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
//    qDebug() << "--Finished EnableDisableMutate()";
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
//    qDebug() << "--Finished CopyMutationRates()";
}

bool Genome::ContainsLink(Gene link)
{
//    qDebug() << "ContainsLink()";
    foreach(Gene *g, genes)
    {
        if(g->into == link.into && g->out == link.out)
        {
//            qDebug() << "--Finished ContainsLink()";
            return true;
        }
    }
//    qDebug() << "--Finished ContainsLink()";
    return false;
}

bool Genome::SameSpecies(const Genome &other, int innovationSize)
{
//    qDebug() << "SameSpecies()";
    float dd = RoboCop::DeltaDisjoint * Disjoint(other.genes, innovationSize);
    float dw = RoboCop::DeltaWeights * Weights(other.genes, innovationSize);
//    qDebug() << "--Finished SameSpecies()";
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
//    qDebug() << "--Finished Disjoint()";
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
//            Gene *gene2 = i2[g->innovation];
            sum += std::abs(g->weight - i2[g->innovation]->weight);
            coincident++;
        }
    }

    i2.clear();

//    qDebug() << "--Finished Weights()";
    return sum / coincident;
}

Genome* Genome::Crossover(Genome *other, int innovationSize)
{
//    qDebug() << "Crossover()";
    if(other->GetFitness() > this->fitness)
    {
        return other->Crossover(this, innovationSize);
    }

    Genome* child = new Genome();

    QMap<int, Gene*> innovations2;

    foreach(Gene *g, other->genes)
    {
        innovations2.insert(g->innovation, g);
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
        n--;
        if( n <= 0 )
        {
//            qDebug() << "--Finished RandomNeuron()";
            return k;
        }
    }

//    qDebug() << "--Finished RandomNeuron()";
    return 0;
}
