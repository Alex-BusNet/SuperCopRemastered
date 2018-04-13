#include "robocophandler.h"
#include <QDebug>
#include <QDir>

RoboCopHandler::RoboCopHandler()
{
    srand(time(0));

    filepath = "States";
    frameUpdate = false;
    gameRunning = false;

    if(!QDir(filepath).exists())
        QDir().mkdir(filepath);

//    qRegisterMetaType<uint8_t>();

}

/*
 * Copy c'tor
 */
RoboCopHandler::RoboCopHandler(const RoboCopHandler &rch) : QThread()
{
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
    while(gameRunning)
    {
        if(frameUpdate)
        {
            frameUpdate = false;

            Species *s = pool->species[pool->GetCurrentSpecies()];
            Genome *g = s->genomes[pool->GetCurrentGenome()];

            if(pool->GetCurrentFrame() % 5 == 0)
                EvaluateCurrent();

            SetControls(controls);

            /// GET POSITIONS

            if (playerPosX > rightmost)
            {
                qDebug() << "\t\tPlayer posX greater than rightmost";
                rightmost = playerPosX;
                timeout = RoboCop::TimeoutConstant;
            }

            timeout--;

            int timeoutBonus = pool->GetCurrentFrame() / 4;

            if(timeout + timeoutBonus <= 0)
            {
                int fitness = rightmost - pool->GetCurrentFrame() / 2;
                if(rightmost > (70 * 205)) // Need to check what rightmost max should be
                    fitness += 1000;

                if(fitness == 0)
                    fitness = -1;

                g->SetFitness(fitness);

                if(fitness > pool->GetMaxFitness())
                {
                    pool->SetMaxFitness(fitness);
                    SaveFile(filepath + "/backup." + pool->GetGeneration() + ".RC_1.json");
                }

                pool->SetCurrentSpecies(0);
                pool->SetCurrentGenome(0);

                while(pool->FitnessAlreadyMeasured())
                    pool->NextGenome();

                InitializeRun();
            }

            //--------------------------
            // NN State info goes here

            emit SpeciesUpdate(pool->GetCurrentSpecies());
            emit GenomeUpdate(pool->GetCurrentGenome());
            emit GenerationUpdate(pool->GetGeneration());
            emit FitnessUpdate(g->GetFitness());
            emit MaxFitnessUpdate(pool->GetMaxFitness());

            //--------------------------

            pool->SetCurrentFrame(pool->GetCurrentFrame() + 1);

            for(long i = 0; i < 1000000L; i++) {;}

            // Advance game frame?
        }
    }
}

void RoboCopHandler::run()
{
    InitializePool();
    gameRunning = true;
    GameLoop();
}

int RoboCopHandler::end(int retcode)
{
    gameRunning = false;
    this->SaveFile(QString("RC_1_Gen%1.json").arg(pool->GetGeneration()));
    this->exit(retcode);
    return 0;
}

void RoboCopHandler::ClearControls()
{
//    qDebug() << "ClearControls()";
    for(int i = 0; i < RoboCop::Outputs; i++)
    {
        controls[RoboCop::ButtonNames[i]] = false;
    }
}

void RoboCopHandler::InitializeRun()
{
//    qDebug() << "InitializeRun()";
    pool->SetCurrentFrame(0);
    timeout = RoboCop::TimeoutConstant;
    emit NewSpecies();
    ClearControls();

    Genome *g = pool->species[pool->GetCurrentSpecies()]->genomes[pool->GetCurrentGenome()];
    g->GenerateNetwork();
    EvaluateCurrent();
//    qDebug() << "--Finished InitializeRun()";
}

void RoboCopHandler::InitializePool()
{
//    qDebug() << "InitializePool()";
    pool = new Pool();
    timeout = 0;
    playerPosX = 0;

    for(int i = 0; i < RoboCop::Population; i++)
    {
        Genome *b = BasicGenome();
        pool->AddToSpecies(b);
    }

    InitializeRun();

//    qDebug() << "--Finished InitializePool()";
}

void RoboCopHandler::SetInputs(int **in)
{
//    qDebug() << "SetInputs()";
    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 18; x++)
        {
            inputs.push_back(in[y][x]);
        }
    }
//    qDebug() << "--Finished SetInputs()";
}

void RoboCopHandler::SetPosition(int x)
{
//    qDebug() << "SetPositions()";
    this->playerPosX = x;
}

void RoboCopHandler::EvaluateCurrent()
{
//    qDebug() << "EvaluateCurrent()";
    controls = pool->EvaluateNetwork(inputs);

    if(controls["LEFT"] && controls["RIGHT"])
    {
        controls["LEFT"] = controls["RIGHT"] = false;
    }

    SetControls(controls);
}

void RoboCopHandler::SetControls(QMap<QString, bool> cState)
{
//    qDebug() << "SetControls()";
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
//    qDebug() << "Mutate()";
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
//    qDebug() << "PointMutate()";
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

    g->ContainsLink(*l);

    l->innovation = pool->NewInnovation() - 1;
    l->weight = ((float)rand() / (float)RAND_MAX) * 4 - 2;

    g->genes.push_back(l);
}

void RoboCopHandler::NodeMutate(Genome *g)
{
//    qDebug() << "NodeMutate()";
    if(g->genes.size() == 0) { return; }

    g->SetMaxNeuron(g->GetMaxNeuron() + 1);

    int idx = rand() % g->genes.size();
    Gene *gene = g->genes[idx];
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
//    qDebug() << "EnableDisableMutate()";
    QVector<Gene*> candidates;

    foreach(Gene *gene, g->genes)
    {
        if(gene->enabled == !enable)
            candidates.push_back(gene);
    }

    if(candidates.size() == 0)
        return;

    Gene *g1 = candidates[rand() % candidates.size()];
    g1->enabled = !g1->enabled;
}

Genome* RoboCopHandler::BasicGenome()
{
//    qDebug() << "BasicGenome()";
    Genome *g = new Genome();
    g->SetMaxNeuron(RoboCop::Inputs);
    Mutate(g);
    return g;
}

Pool* RoboCopHandler::GetPool()
{
//    qDebug() << "GetPool()";
    return pool;
}

QVector<int> RoboCopHandler::GetInputs()
{
//    qDebug() << "GetInputs()";
    return inputs;
}

int RoboCopHandler::GetPlayerPosX()
{
//    qDebug() << "GetPlayerPosX()";
    return playerPosX;
}

int RoboCopHandler::GetRightMost()
{
//    qDebug() << "GetRightMost()";
    return rightmost;
}

int RoboCopHandler::GetTimeout()
{
//    qDebug() << "GetTimeout()";
    return timeout;
}

QMap<QString, bool> RoboCopHandler::GetControls()
{
//    qDebug() << "GetControls()";
    return controls;
}

void RoboCopHandler::SaveFile(QString filename)
{
    QFile saveState(filename);

    if(saveState.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc;
        QJsonObject obj;

        pool->SavePool(obj);

        doc.setObject(obj);

        saveState.write(doc.toJson());

        saveState.flush();
        saveState.close();
    }
}

void RoboCopHandler::LoadFile(QString filename)
{
    QFile saveState(filename);

    if(saveState.exists())
    {
        if(saveState.open(QIODevice::ReadOnly))
        {
            QJsonDocument doc = QJsonDocument::fromJson(saveState.readAll());
            QJsonObject obj = doc.object();
            pool->LoadPool(obj);
        }
    }

    while(pool->FitnessAlreadyMeasured())
    {
        pool->NextGenome();
    }

    InitializeRun();
    pool->SetCurrentFrame(pool->GetCurrentFrame() + 1);
}

void RoboCopHandler::FrameUpdated()
{
    frameUpdate = true;
}

