#include "robocophandler.h"
#include <QDebug>

RoboCopHandler::RoboCopHandler()
{
    srand(time(0));
    pool = new Pool();
    timeout = 0;
    playerPosX = 0;
    for(int i = 0; i < RoboCop::Population; i++)
    {
        Genome *b = BasicGenome();
        pool->AddToSpecies(b);
    }

    InitializeRun();
}

RoboCopHandler::RoboCopHandler(const RoboCopHandler &rch)
{
    /*
     * Copy c'tor
     */

    pool = rch.pool;
    inputs = rch.inputs;
    controls = rch.controls;
    timeout = rch.timeout;
    playerPosX = rch.playerPosX;
    rightmost = rch.rightmost;
}

RoboCopHandler::~RoboCopHandler()
{

}

void RoboCopHandler::GameLoop()
{
    while(true)
    {
        Species *s = pool->species[pool->GetCurrentSpecies()];
        Genome *g = s->genomes[pool->GetCurrentGenome()];

        if(pool->GetCurrentFrame() % 5 == 0)
            EvaluateCurrent();

        SetControls(controls);

        if (playerPosX > rightmost)
        {
            rightmost = playerPosX;
            timeout = RoboCop::TimeoutConstant;
        }

        timeout--;

        int timeoutBonus = pool->GetCurrentFrame() / 4;

        if(timeout + timeoutBonus <= 0)
        {
            int fitness = rightmost - pool->GetCurrentFrame() / 2;
            if(rightmost > (70 * 205)) // Need to check what rightmost max should be
                fitness + 1000;

            if(fitness == 0)
                fitness = -1;

            g->SetFitness(fitness);

            if(fitness > pool->GetMaxFitness())
            {
                pool->SetMaxFitness(fitness);
                //Write backup file?
            }

            pool->SetCurrentSpecies(0);
            pool->SetCurrentGenome(0);

            while(pool->FitnessAlreadyMeasured())
                pool->NextGenome();

            InitializeRun();
        }

        //--------------------------
        // NN State info goes here
        //--------------------------

        pool->SetCurrentFrame(pool->GetCurrentFrame() + 1);

        // Advance game frame?
    }
}

void RoboCopHandler::ClearControls()
{
    for(int i = 0; i < RoboCop::Outputs; i++)
    {
        controls[RoboCop::ButtonNames[i]] = false;
    }
}

void RoboCopHandler::InitializeRun()
{
    pool->SetCurrentFrame(0);
    timeout = RoboCop::TimeoutConstant;
    ClearControls();

    Genome *g = pool->species[pool->GetCurrentSpecies()]->genomes[pool->GetCurrentGenome()];
    g->GenerateNetwork();
//    EvaluateCurrent();
}

void RoboCopHandler::SetInputs(int **in)
{
    qDebug() << "Setting inputs";
    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 18; x++)
        {
            inputs.push_back(in[y][x]);
        }
    }
    qDebug() << "Done";
}

void RoboCopHandler::SetPosition(int x)
{
    this->playerPosX = x;
}

void RoboCopHandler::EvaluateCurrent()
{
    controls = pool->EvaluateNetwork(inputs);

    if(controls["LEFT"] && controls["RIGHT"])
    {
        controls["LEFT"] = controls["RIGHT"] = false;
    };
}

void RoboCopHandler::SetControls(QMap<QString, bool> cState)
{
    controls = cState;
    uint8_t keyPressState = 0b0000;

    if(cState["JUMP"])
        keyPressState |= 0b0100;

    if(cState["RIGHT"])
        keyPressState |= 0b0010;

    if(cState["LEFT"])
        keyPressState |= 0b0001;

    if(cState["SPRINT"])
        keyPressState |= 0b1000;

    emit keyStateUpdate(keyPressState);
}

void RoboCopHandler::Mutate(Genome *g)
{
    foreach(QString s, g->mutationRates.keys())
    {
        if((rand() % 2) == 1)
            g->mutationRates[s] *= 0.95f; // Values from MarI/O
        else
            g->mutationRates[s] *= 1.05263f; // Values from MarI/O
    }

    float p = g->mutationRates["connections"];

    if(((float)rand() / (float)RAND_MAX) < p)
        PointMutate(g);

    p = g->mutationRates["link"];
    while (p > 0)
    {
        if (((float)rand() / (float)RAND_MAX) < p)
            LinkMutate(g, false);

        p -= 1;
    }

    p = g->mutationRates["bias"];
    while(p > 0)
    {
        if (((float)rand() / (float)RAND_MAX) < p)
            LinkMutate(g, true);

        p -= 1;
    }

    p = g->mutationRates["node"];
    while(p > 0)
    {
        if (((float)rand() / (float)RAND_MAX) < p)
            NodeMutate(g);

        p -= 1;
    }

    p = g->mutationRates["enable"];
    while(p > 0)
    {
        if (((float)rand() / (float)RAND_MAX) < p)
            EnableDisableMutate(g, true);

        p -= 1;
    }

    p = g->mutationRates["disable"];
    while(p > 0)
    {
        if (((float)rand() / (float)RAND_MAX) < p)
            EnableDisableMutate(g, false);

        p -= 1;
    }
}

void RoboCopHandler::PointMutate(Genome *g)
{
    float step = g->GetStep();

    for(int i = 0; i < g->genes.size(); i++)
    {
        Gene *gene = g->genes[i];
        float p = ((float)rand() / (float)RAND_MAX);
        if(p < RoboCop::PerturbChance)
            gene->weight += (((float)rand() / (float)RAND_MAX) * step * 2) - step;
        else
            gene->weight = ((float)rand() / (float)RAND_MAX) * 4 - 2;
    }
}

void RoboCopHandler::LinkMutate(Genome *g, bool bias)
{
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

    g->ContainsLink(*l);

    l->innovation = pool->NewInnovation();
    l->weight = ((float)rand() / (float)RAND_MAX) * 4 - 2;

    g->genes.push_back(l);
}

void RoboCopHandler::NodeMutate(Genome *g)
{
    if(g->genes.size() == 0) { return; }

    g->SetMaxNeuron(g->GetMaxNeuron() + 1);

    Gene *gene = g->genes[rand() % g->genes.size()];
    if(!gene->enabled) { return; }

    gene->enabled = false;

    Gene *g1 = new Gene(*gene);
    g1->out = g->GetMaxNeuron();
    g1->weight = 1.0;
    g1->innovation = pool->NewInnovation();
    g1->enabled = true;
    g->genes.push_back(g1);

    Gene *g2 = new Gene(*gene);
    g2->into = g->GetMaxNeuron();
    g2->innovation = pool->NewInnovation();
    g2->enabled = true;;
    g->genes.push_back(g2);
}

void RoboCopHandler::EnableDisableMutate(Genome *g, bool enable)
{
    QVector<Gene*> candidates;

    foreach(Gene *gene, g->genes)
    {
        if(gene->enabled == !enable)
            candidates.push_back(gene);
    }

    if(candidates.size() == 0)
        return;

    Gene *g1 = candidates[rand() % candidates.size()];
    g1->enabled != g1->enabled;
}

Genome* RoboCopHandler::BasicGenome()
{
    Genome *g = new Genome();
    g->SetMaxNeuron(RoboCop::Inputs);
    Mutate(g);
    return g;
}

Pool* RoboCopHandler::GetPool()
{
    return pool;
}

QVector<int> RoboCopHandler::GetInputs()
{
    return inputs;
}

int RoboCopHandler::GetPlayerPosX()
{
    return playerPosX;
}

int RoboCopHandler::GetRightMost()
{
    return rightmost;
}

int RoboCopHandler::GetTimeout()
{
    return timeout;
}

QMap<QString, bool> RoboCopHandler::GetControls()
{
    return controls;
}

